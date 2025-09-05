#!/usr/bin/env python3
"""
Enhanced FluentAutoCarousel Test Runner

This script runs the comprehensive test suite for the enhanced FluentAutoCarousel
component, including all the new functionality we added:

1. Fluent UI Design Compliance tests
2. Complete Interface Implementation tests  
3. Code Quality tests
4. Integration and performance tests

Usage:
    python run_enhanced_carousel_tests.py [options]

Options:
    --build-dir DIR     Build directory (default: build)
    --verbose          Show detailed test output
    --coverage         Generate coverage report
    --performance      Run performance benchmarks
    --xml-output       Generate XML test reports
    --help             Show this help message
"""

import argparse
import os
import subprocess
import sys
import time
from pathlib import Path


class EnhancedCarouselTestRunner:
    def __init__(self, build_dir="build"):
        self.build_dir = Path(build_dir)
        self.test_dir = self.build_dir / "tests"
        self.results = {}
        
    def run_test(self, test_name, verbose=False, xml_output=False):
        """Run a specific test and return the result."""
        test_executable = self.test_dir / test_name
        
        if not test_executable.exists():
            print(f"❌ Test executable not found: {test_executable}")
            return False
            
        cmd = [str(test_executable)]
        
        # Add Qt test options
        if xml_output:
            xml_file = self.test_dir / f"{test_name}_results.xml"
            cmd.extend(["-xml", "-o", str(xml_file)])
        
        if verbose:
            cmd.append("-v2")  # Verbose output
        else:
            cmd.append("-silent")  # Minimal output
            
        try:
            print(f"🧪 Running {test_name}...")
            start_time = time.time()
            
            result = subprocess.run(
                cmd, 
                capture_output=True, 
                text=True, 
                timeout=60  # 60 second timeout
            )
            
            end_time = time.time()
            duration = end_time - start_time
            
            success = result.returncode == 0
            
            self.results[test_name] = {
                'success': success,
                'duration': duration,
                'stdout': result.stdout,
                'stderr': result.stderr
            }
            
            if success:
                print(f"✅ {test_name} passed ({duration:.2f}s)")
            else:
                print(f"❌ {test_name} failed ({duration:.2f}s)")
                if verbose:
                    print(f"   stdout: {result.stdout}")
                    print(f"   stderr: {result.stderr}")
                    
            return success
            
        except subprocess.TimeoutExpired:
            print(f"⏰ {test_name} timed out")
            return False
        except Exception as e:
            print(f"💥 {test_name} crashed: {e}")
            return False
    
    def run_enhanced_tests(self, verbose=False, xml_output=False):
        """Run all enhanced FluentAutoCarousel tests."""
        tests = [
            "FluentAutoCarouselEnhancedTest",  # Our new comprehensive test
            "FluentAutoCarouselAPITest",       # Existing API test
            "FluentAutoCarouselTest"           # Existing functionality test
        ]
        
        print("🚀 Running Enhanced FluentAutoCarousel Test Suite")
        print("=" * 60)
        
        passed = 0
        total = len(tests)
        
        for test in tests:
            if self.run_test(test, verbose, xml_output):
                passed += 1
        
        print("\n" + "=" * 60)
        print(f"📊 Test Results: {passed}/{total} tests passed")
        
        if passed == total:
            print("🎉 All tests passed!")
            return True
        else:
            print(f"😞 {total - passed} tests failed")
            return False
    
    def generate_coverage_report(self):
        """Generate code coverage report if coverage tools are available."""
        try:
            # Check if gcov/lcov are available
            subprocess.run(["gcov", "--version"], capture_output=True, check=True)
            subprocess.run(["lcov", "--version"], capture_output=True, check=True)
            
            print("📈 Generating coverage report...")
            
            # Generate coverage data
            subprocess.run([
                "lcov", "--capture", "--directory", str(self.build_dir),
                "--output-file", str(self.build_dir / "coverage.info")
            ], check=True)
            
            # Generate HTML report
            coverage_dir = self.build_dir / "coverage_html"
            subprocess.run([
                "genhtml", str(self.build_dir / "coverage.info"),
                "--output-directory", str(coverage_dir)
            ], check=True)
            
            print(f"✅ Coverage report generated: {coverage_dir}/index.html")
            return True
            
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("⚠️  Coverage tools not available (gcov/lcov)")
            return False
    
    def run_performance_benchmarks(self):
        """Run performance benchmarks for the enhanced carousel."""
        print("🏃 Running performance benchmarks...")
        
        # Create a simple performance test
        perf_test_code = '''
        #include <QtTest/QtTest>
        #include <QElapsedTimer>
        #include "FluentQt/Components/FluentAutoCarousel.h"
        
        class PerfTest : public QObject {
            Q_OBJECT
        private slots:
            void benchmarkSizeHint() {
                FluentAutoCarousel carousel;
                for (int i = 0; i < 10; ++i) {
                    carousel.addItem(new QWidget());
                }
                
                QBENCHMARK {
                    carousel.sizeHint();
                }
            }
        };
        
        QTEST_MAIN(PerfTest)
        #include "perf_test.moc"
        '''
        
        # This is a placeholder - in a real implementation, you'd compile and run this
        print("⚠️  Performance benchmarks require separate compilation")
        return True
    
    def print_summary(self):
        """Print a detailed summary of test results."""
        if not self.results:
            print("No test results to summarize")
            return
            
        print("\n" + "=" * 60)
        print("📋 DETAILED TEST SUMMARY")
        print("=" * 60)
        
        total_duration = 0
        for test_name, result in self.results.items():
            status = "✅ PASS" if result['success'] else "❌ FAIL"
            duration = result['duration']
            total_duration += duration
            
            print(f"{status} {test_name:<30} ({duration:.2f}s)")
            
        print(f"\nTotal execution time: {total_duration:.2f}s")
        
        # Show failed tests details
        failed_tests = [name for name, result in self.results.items() if not result['success']]
        if failed_tests:
            print(f"\n❌ Failed tests ({len(failed_tests)}):")
            for test_name in failed_tests:
                result = self.results[test_name]
                print(f"  • {test_name}")
                if result['stderr']:
                    print(f"    Error: {result['stderr'][:100]}...")


def main():
    parser = argparse.ArgumentParser(
        description="Enhanced FluentAutoCarousel Test Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    parser.add_argument(
        "--build-dir", 
        default="build", 
        help="Build directory (default: build)"
    )
    parser.add_argument(
        "--verbose", 
        action="store_true", 
        help="Show detailed test output"
    )
    parser.add_argument(
        "--coverage", 
        action="store_true", 
        help="Generate coverage report"
    )
    parser.add_argument(
        "--performance", 
        action="store_true", 
        help="Run performance benchmarks"
    )
    parser.add_argument(
        "--xml-output", 
        action="store_true", 
        help="Generate XML test reports"
    )
    
    args = parser.parse_args()
    
    # Create test runner
    runner = EnhancedCarouselTestRunner(args.build_dir)
    
    # Check if build directory exists
    if not runner.build_dir.exists():
        print(f"❌ Build directory not found: {runner.build_dir}")
        print("Please build the project first:")
        print("  mkdir build && cd build")
        print("  cmake .. && make")
        return 1
    
    # Run the tests
    success = runner.run_enhanced_tests(args.verbose, args.xml_output)
    
    # Generate coverage report if requested
    if args.coverage:
        runner.generate_coverage_report()
    
    # Run performance benchmarks if requested
    if args.performance:
        runner.run_performance_benchmarks()
    
    # Print detailed summary
    runner.print_summary()
    
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())
