#!/usr/bin/env python3
"""
FluentQt Test Runner

A comprehensive test runner for the FluentQt test suite with support for:
- Running individual tests or test suites
- Generating coverage reports
- Running performance benchmarks
- Memory leak detection
- Test result reporting
- CI/CD integration
"""

import argparse
import os
import subprocess
import sys
import json
import time
from pathlib import Path
from typing import List, Dict, Optional

class TestRunner:
    def __init__(self, build_dir: str = "build"):
        self.build_dir = Path(build_dir)
        self.test_dir = self.build_dir / "tests"
        self.results = {}
        
    def run_test(self, test_name: str, timeout: int = 30) -> Dict:
        """Run a single test and return results."""
        test_executable = self.test_dir / test_name
        
        if not test_executable.exists():
            return {
                "name": test_name,
                "status": "FAILED",
                "error": f"Test executable not found: {test_executable}",
                "duration": 0
            }
        
        print(f"Running {test_name}...")
        start_time = time.time()
        
        try:
            result = subprocess.run(
                [str(test_executable)],
                capture_output=True,
                text=True,
                timeout=timeout,
                env={**os.environ, "QT_QPA_PLATFORM": "offscreen"}
            )
            
            duration = time.time() - start_time
            
            return {
                "name": test_name,
                "status": "PASSED" if result.returncode == 0 else "FAILED",
                "returncode": result.returncode,
                "stdout": result.stdout,
                "stderr": result.stderr,
                "duration": duration
            }
            
        except subprocess.TimeoutExpired:
            return {
                "name": test_name,
                "status": "TIMEOUT",
                "error": f"Test timed out after {timeout} seconds",
                "duration": timeout
            }
        except Exception as e:
            return {
                "name": test_name,
                "status": "ERROR",
                "error": str(e),
                "duration": time.time() - start_time
            }
    
    def run_all_tests(self, pattern: str = "*Test") -> Dict:
        """Run all tests matching the pattern."""
        test_executables = list(self.test_dir.glob(pattern))
        
        if not test_executables:
            print(f"No test executables found matching pattern: {pattern}")
            return {}
        
        results = {}
        total_tests = len(test_executables)
        passed = 0
        failed = 0
        
        print(f"Found {total_tests} test(s) to run")
        print("=" * 60)
        
        for i, test_exe in enumerate(test_executables, 1):
            test_name = test_exe.name
            print(f"[{i}/{total_tests}] ", end="")
            
            result = self.run_test(test_name)
            results[test_name] = result
            
            if result["status"] == "PASSED":
                passed += 1
                print(f"✓ {test_name} ({result['duration']:.2f}s)")
            else:
                failed += 1
                print(f"✗ {test_name} - {result['status']}")
                if "error" in result:
                    print(f"  Error: {result['error']}")
        
        print("=" * 60)
        print(f"Results: {passed} passed, {failed} failed, {total_tests} total")
        
        return results
    
    def generate_coverage_report(self) -> bool:
        """Generate code coverage report."""
        print("Generating coverage report...")
        
        try:
            # Run coverage target
            result = subprocess.run(
                ["make", "coverage"],
                cwd=self.build_dir,
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                print("✓ Coverage report generated successfully")
                coverage_html = self.build_dir / "coverage" / "html" / "index.html"
                if coverage_html.exists():
                    print(f"Coverage report available at: {coverage_html}")
                return True
            else:
                print(f"✗ Coverage generation failed: {result.stderr}")
                return False
                
        except Exception as e:
            print(f"✗ Coverage generation error: {e}")
            return False
    
    def run_memory_check(self, test_name: str) -> Dict:
        """Run memory leak detection with Valgrind."""
        print(f"Running memory check for {test_name}...")
        
        test_executable = self.test_dir / test_name
        
        try:
            result = subprocess.run([
                "valgrind",
                "--tool=memcheck",
                "--leak-check=full",
                "--show-leak-kinds=all",
                "--track-origins=yes",
                "--xml=yes",
                f"--xml-file={self.build_dir}/valgrind_{test_name}.xml",
                str(test_executable)
            ], capture_output=True, text=True, timeout=120)
            
            return {
                "name": test_name,
                "status": "COMPLETED",
                "valgrind_output": result.stderr,
                "xml_report": f"{self.build_dir}/valgrind_{test_name}.xml"
            }
            
        except subprocess.TimeoutExpired:
            return {
                "name": test_name,
                "status": "TIMEOUT",
                "error": "Memory check timed out"
            }
        except FileNotFoundError:
            return {
                "name": test_name,
                "status": "SKIPPED",
                "error": "Valgrind not found"
            }
        except Exception as e:
            return {
                "name": test_name,
                "status": "ERROR",
                "error": str(e)
            }
    
    def run_benchmarks(self) -> Dict:
        """Run performance benchmarks."""
        print("Running performance benchmarks...")
        
        try:
            result = subprocess.run(
                ["make", "benchmark"],
                cwd=self.build_dir,
                capture_output=True,
                text=True
            )
            
            return {
                "status": "COMPLETED" if result.returncode == 0 else "FAILED",
                "output": result.stdout,
                "error": result.stderr if result.returncode != 0 else None
            }
            
        except Exception as e:
            return {
                "status": "ERROR",
                "error": str(e)
            }
    
    def generate_report(self, results: Dict, output_file: str = "test_report.json"):
        """Generate a comprehensive test report."""
        report = {
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
            "build_dir": str(self.build_dir),
            "summary": {
                "total": len(results),
                "passed": sum(1 for r in results.values() if r["status"] == "PASSED"),
                "failed": sum(1 for r in results.values() if r["status"] == "FAILED"),
                "timeout": sum(1 for r in results.values() if r["status"] == "TIMEOUT"),
                "error": sum(1 for r in results.values() if r["status"] == "ERROR"),
                "total_duration": sum(r.get("duration", 0) for r in results.values())
            },
            "tests": results
        }
        
        output_path = self.build_dir / output_file
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Test report saved to: {output_path}")
        return report

def main():
    parser = argparse.ArgumentParser(description="FluentQt Test Runner")
    parser.add_argument("--build-dir", default="build", help="Build directory path")
    parser.add_argument("--test", help="Run specific test")
    parser.add_argument("--pattern", default="*Test", help="Test pattern to match")
    parser.add_argument("--coverage", action="store_true", help="Generate coverage report")
    parser.add_argument("--memcheck", help="Run memory check on specific test")
    parser.add_argument("--benchmark", action="store_true", help="Run performance benchmarks")
    parser.add_argument("--timeout", type=int, default=30, help="Test timeout in seconds")
    parser.add_argument("--report", default="test_report.json", help="Output report file")
    parser.add_argument("--verbose", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    runner = TestRunner(args.build_dir)
    
    if not runner.test_dir.exists():
        print(f"Error: Test directory not found: {runner.test_dir}")
        print("Make sure you have built the tests first.")
        sys.exit(1)
    
    results = {}
    
    # Run specific test
    if args.test:
        result = runner.run_test(args.test, args.timeout)
        results[args.test] = result
        
        if args.verbose and result["status"] != "PASSED":
            print(f"STDOUT:\n{result.get('stdout', '')}")
            print(f"STDERR:\n{result.get('stderr', '')}")
    
    # Run all tests
    else:
        results = runner.run_all_tests(args.pattern)
    
    # Generate coverage report
    if args.coverage:
        runner.generate_coverage_report()
    
    # Run memory check
    if args.memcheck:
        memcheck_result = runner.run_memory_check(args.memcheck)
        print(f"Memory check result: {memcheck_result['status']}")
    
    # Run benchmarks
    if args.benchmark:
        benchmark_result = runner.run_benchmarks()
        print(f"Benchmark result: {benchmark_result['status']}")
    
    # Generate report
    if results:
        report = runner.generate_report(results, args.report)
        
        # Exit with error code if any tests failed
        if report["summary"]["failed"] > 0 or report["summary"]["error"] > 0:
            sys.exit(1)

if __name__ == "__main__":
    main()
