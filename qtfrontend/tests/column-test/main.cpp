#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <QApplication>
#include <QMainWindow>

class globals
{
	public:
		static QApplication * app;
		static QMainWindow * mw;
		
};

QApplication * globals::app;
QMainWindow * globals::mw;

int main(int argc, char **argv)
{
	globals::app = new QApplication(argc, argv);
	globals::mw = new QMainWindow();

	CppUnit::TestResult result;
	CppUnit::TestResultCollector collector;
	CppUnit::BriefTestProgressListener listener;
	result.addListener(&collector);
	result.addListener(&listener);

	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
	runner.run(result);

	CppUnit::CompilerOutputter out(&collector, CppUnit::stdCOut());
	out.write();
	return collector.wasSuccessful() ? 0 : 1;
}

