/**
Copyright (c) 2009, Brian Cronin
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions
and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or
promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

//NOTE: Because the tests deal with time, they are designed to pass in release mode and so may fail in debug mode.
//Effort as been made to make them pass in debug mode as well however.

#include "UnitTest++.h"
#include "SuperProfiler.h"
#include "SuperFunctionData.h"
#include "SuperUtils.h"
#include "SuperOutputTest.h"
#include "SuperOutputText.h"
#include "SuperOutputCSV.h"
#include "SuperOutputXML.h"
#include <iostream>
#include <fstream>


SUITE(SuperProfilerTests)
{
	void TestFunctionCall1(float & byRef)
	{
		SUPER_PROFILE("TestFunctionCall1(float &)");

		UnitTest::TimeHelpers::SleepMs(10);
	}


	void TestFunctionCall2(int p1, bool p2)
	{
		SUPER_PROFILE("TestFunctionCall2(int, bool)");

		UnitTest::TimeHelpers::SleepMs(20);
	}


	void TestFunctionCall3(void)
	{
		SUPER_PROFILE("TestFunctionCall3()");

		float data = 0;
		TestFunctionCall1(data);

		UnitTest::TimeHelpers::SleepMs(30);
	}


	void TestFunctionCall4(void)
	{
		SUPER_PROFILE("TestFunctionCall4()");

		TestFunctionCall3();
		TestFunctionCall2(1, true);

		UnitTest::TimeHelpers::SleepMs(40);
	}


	void TestFunctionCallRecursion1(int & i)
	{
		SUPER_PROFILE("TestFunctionCallRecursion1(int &)");

		UnitTest::TimeHelpers::SleepMs(10);

		if (i >= 4)
		{
			return;
		}

		i++;
		TestFunctionCallRecursion1(i);
	}


	//This is to test the SuperStack without SuperRoot
	TEST(SuperStack_Alone)
	{
		SuperProfiler::SuperTimer testTimer;
		SuperProfiler::SuperStack testStack;

		//First create some test functions
		SuperProfiler::SuperFunctionData mainLoop("MainLoop()");
		SuperProfiler::SuperFunctionData apple("Apple()");
		SuperProfiler::SuperFunctionData orange("Orange()");
		SuperProfiler::SuperFunctionData peach("Peach()");

		//Add some test data
		testStack.Push(&mainLoop, testTimer.GetTimeSeconds());
		{
			//Child 1
			testStack.Push(&apple, testTimer.GetTimeSeconds());
			{
				//Child 1
				testStack.Push(&orange, testTimer.GetTimeSeconds());
				{
				}
				testStack.Pop(testTimer.GetTimeSeconds());
				//Child 1 again
				testStack.Push(&orange, testTimer.GetTimeSeconds());
				{
					//Child 1
					testStack.Push(&peach, testTimer.GetTimeSeconds());
					{
					}
					testStack.Pop(testTimer.GetTimeSeconds());
				}
				testStack.Pop(testTimer.GetTimeSeconds());
			}
			testStack.Pop(testTimer.GetTimeSeconds());
		}
		testStack.Pop(testTimer.GetTimeSeconds());

		//Make sure the stack is at the proper depth
		CHECK_EQUAL(0, testStack.GetCurrentDepth());

		//Confirm the stack is correct
		CHECK_EQUAL(1, testStack.GetNumChildren());
		SuperProfiler::SuperStackNode * mainLoopNode = testStack.GetChild(&mainLoop);
		CHECK_EQUAL("MainLoop()", mainLoopNode->GetFuncData()->GetName());
		CHECK(mainLoopNode != NULL);
		{
			CHECK_EQUAL(1, mainLoopNode->GetNumChildren());
			SuperProfiler::SuperStackNode * appleNode = mainLoopNode->GetChild(&apple);
			CHECK(appleNode != NULL);
			{
				CHECK_EQUAL(1, appleNode->GetNumChildren());
				SuperProfiler::SuperStackNode * orangeNode = appleNode->GetChild(&orange);
				CHECK(orangeNode != NULL);
				{
					CHECK_EQUAL(1, orangeNode->GetNumChildren());
					SuperProfiler::SuperStackNode * peachNode = orangeNode->GetChild(&peach);
					CHECK(peachNode != NULL);
					{
						CHECK_EQUAL(0, peachNode->GetNumChildren());
					}
				}
			}
		}
	}


	TEST(Unused_SuperOutputTest)
	{
		//Create a default test output object
		SuperProfiler::SuperOutputTest superOutputTest;

		//Nothing written to the output, so it's values should be at default
		CHECK_EQUAL(0, superOutputTest.GetMaxDepth());
	}


	TEST(No_Calls_SuperOutputTest)
	{
		SuperProfiler::SuperRoot::Reset();

		//Create a default test output object
		SuperProfiler::SuperOutputTest superOutputTest;
		SuperProfiler::SuperRoot::OutputResults(superOutputTest);

		//Nothing written to the output, so it's values should be at default
		CHECK_EQUAL(0, superOutputTest.GetMaxDepth());
	}


	TEST(One_Call_SuperOutputTest)
	{
		SuperProfiler::SuperRoot::Reset();

		float data = 0;
		TestFunctionCall1(data);

		//Create a default test output object
		SuperProfiler::SuperOutputTest superOutputTest;
		SuperProfiler::SuperRoot::OutputResults(superOutputTest);

		//One function call
		CHECK_EQUAL(1, superOutputTest.GetMaxDepth());
	}


	TEST(Complex_SuperOutputTest)
	{
		SuperProfiler::SuperRoot::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		//Create a default test output object
		SuperProfiler::SuperOutputTest superOutputTest;
		SuperProfiler::SuperRoot::OutputResults(superOutputTest);

		//Make sure the total run time is about what we expect
		CHECK_CLOSE(0.2, superOutputTest.GetTotalRunTime(), 0.05);

		//Make sure MAIN() used 100% of the time
		CHECK_EQUAL(superOutputTest.GetTotalRunTime(), superOutputTest.GetFunctionRunTime("MAIN()"));

		//Make sure TestFunctionCall4() was called twice
		CHECK_EQUAL(2, superOutputTest.GetFunctionCallAmount("TestFunctionCall4()"));

		//Check that TestFunctionCall4() in the call tree was called twice
		CHECK_EQUAL(2, superOutputTest.GetCallTree().GetFunction("MAIN()")->GetFunction("TestFunctionCall4()")->GetCallAmount());

		//Check that TestFunctionCall4() in the call tree took 200ms
		CHECK_CLOSE(0.2, superOutputTest.GetCallTree().GetFunction("MAIN()")->GetFunction("TestFunctionCall4()")->GetRunTime(), 0.05);

		//Check that TestFunctionCall4() in the call tree took 100ms on avg
		CHECK_CLOSE(0.1, superOutputTest.GetCallTree().GetFunction("MAIN()")->GetFunction("TestFunctionCall4()")->GetAvgRunTime(), 0.05);
	}


	TEST(SuperOutputText_Invalid_Results)
	{
		SuperProfiler::SuperRoot::Reset();

		//Invalid profile since it is in the same scope as the SuperProfiler::SuperRoot::OutputResults() call.
		SUPER_PROFILE("MAIN()");

		TestFunctionCall4();

		//Create a default SuperOutputText object
		SuperProfiler::SuperOutputText superOutputText("InvalidTXTResults.txt");
		//This should fail since the above MAIN() profile is in the same scope as us
		CHECK_EQUAL(false, SuperProfiler::SuperRoot::OutputResults(superOutputText));

		//File should be empty
		std::ifstream ifs("InvalidTXTResults.txt", std::ifstream::in);
		ifs.seekg(0, std::ios::end);
		int fileLength = ifs.tellg();
		CHECK_EQUAL(0, fileLength);
	}


	TEST(SuperOutputText_Valid_Results)
	{
		SuperProfiler::SuperRoot::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		//Create a default SuperOutputText object
		SuperProfiler::SuperOutputText superOutputText("ValidTXTResults.txt");
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(superOutputText));
	}


	TEST(CSVOutput_Valid_Results)
	{
		SuperProfiler::SuperRoot::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		SuperProfiler::SuperOutputCSV csvOutput("ValidCSVResults.csv");
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(csvOutput));
	}


	TEST(Recursion_Function_Call)
	{
		SuperProfiler::SuperRoot::Reset();

		int i = 0;
		TestFunctionCallRecursion1(i);

		//Create a default test output object
		SuperProfiler::SuperOutputTest SuperOutputTest;
		SuperProfiler::SuperRoot::OutputResults(SuperOutputTest);

		CHECK_EQUAL(5, SuperOutputTest.GetMaxDepth());
	}


	TEST(Two_Outputs)
	{
		SuperProfiler::SuperRoot::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		SuperProfiler::SuperOutputText superOutputText("TwoOutputsTXTResults.txt");
		SuperProfiler::SuperOutputCSV csvOutput("TwoOutputsCSVResults.csv");
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(superOutputText));
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(csvOutput));
	}


	TEST(Three_Outputs)
	{
		SuperProfiler::SuperRoot::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		SuperProfiler::SuperOutputText superOutputText("ThreeOutputsTXTResults.txt");
		SuperProfiler::SuperOutputCSV csvOutput("ThreeOutputsCSVResults.csv");
		SuperProfiler::SuperOutputXML xmlOutput("ThreeOutputsXMLResults.xml");
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(superOutputText));
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(csvOutput));
		CHECK_EQUAL(true, SuperProfiler::SuperRoot::OutputResults(xmlOutput));
	}


	TEST(Util_SearchAndReplaceTest)
	{
		//Replace a character with a string that contains the replaced character
		std::string testData1("Hello(int &, float &, Apple &)");
		std::string testExpectedResults1("Hello(int &amp;, float &amp;, Apple &amp;)");
		std::string testResult1 = SuperProfiler::SuperUtils::FindAndReplace(testData1, "&", "&amp;");
		CHECK_EQUAL(testExpectedResults1, testResult1);

		//Replace a string with a string
		std::string testData2("Garlic Tomato Garlic");
		std::string testExpectedResults2("Tomato Tomato Tomato");
		std::string testResult2 = SuperProfiler::SuperUtils::FindAndReplace(testData2, "Garlic", "Tomato");
		CHECK_EQUAL(testExpectedResults2, testResult2);

		//Replace a string with nothing
		std::string testData3("La1La2La, La1La2Mo");
		std::string testExpectedResults3(", La1La2Mo");
		std::string testResult3 = SuperProfiler::SuperUtils::FindAndReplace(testData3, "La1La2La", "");
		CHECK_EQUAL(testExpectedResults3, testResult3);

		//Replace nothing with a string
		std::string testData4("Don't replace me :(");
		std::string testExpectedResults4("Don't replace me :(");
		std::string testResult4 = SuperProfiler::SuperUtils::FindAndReplace(testData4, "", "You have been replaced! :(");
		CHECK_EQUAL(testExpectedResults4, testResult4);

		//Empty search string
		std::string testData5("");
		std::string testExpectedResults5("");
		std::string testResult5 = SuperProfiler::SuperUtils::FindAndReplace(testData5, "Hello", "Goodbye");
		CHECK_EQUAL(testExpectedResults5, testResult5);

		//Replace a string with a character
		std::string testData6("Hello Mall");
		std::string testExpectedResults6("Helo Mal");
		std::string testResult6 = SuperProfiler::SuperUtils::FindAndReplace(testData6, "ll", "l");
		CHECK_EQUAL(testExpectedResults6, testResult6);
	}
}


int main(int, char const * [])
{
	return UnitTest::RunAllTests();
}