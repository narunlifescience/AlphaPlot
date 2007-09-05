#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

int main()
{
	CppUnit::TestResult result;
	CppUnit::TestResultCollector collector;
	result.addListener(&collector);

	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
	runner.run(result);

	CppUnit::CompilerOutputter out(&collector, CppUnit::stdCOut());
	out.write();
	return collector.wasSuccessful() ? 0 : 1;
}

