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


#include "UnitTest++.h"
#include "SuperProfiler.h"
#include "SuperFunctionData.h"
#include "TestOutput.h"
#include "TextOutput.h"
#include <iostream>
#include <fstream>


SUITE(SuperProfilerTests)
{
	void TestFunctionCall1(void)
	{
		SUPER_PROFILE("void TestFunctionCall1(void)");

		UnitTest::TimeHelpers::SleepMs(500);
	}


	void TestFunctionCall2(void)
	{
		SUPER_PROFILE("void TestFunctionCall2(void)");

		UnitTest::TimeHelpers::SleepMs(25);
	}


	void TestFunctionCall3(void)
	{
		SUPER_PROFILE("void TestFunctionCall3(void)");

		TestFunctionCall1();

		UnitTest::TimeHelpers::SleepMs(50);
	}


	void TestFunctionCall4(void)
	{
		SUPER_PROFILE("void TestFunctionCall4(void)");

		TestFunctionCall3();
		TestFunctionCall2();

		UnitTest::TimeHelpers::SleepMs(100);
	}


	void TestFunctionCallRecursion1(int & i)
	{
		SUPER_PROFILE("void TestFunctionCallRecursion1(int & i)");

		UnitTest::TimeHelpers::SleepMs(100);

		if (i >= 4)
		{
			return;
		}

		i++;
		TestFunctionCallRecursion1(i);
	}


	//This is to test the SuperStack without Root
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


	TEST(Unused_TestOutput)
	{
		//Create a default test output object
		SuperProfiler::TestOutput testOutput;

		//Nothing written to the output, so it's values should be at default
		CHECK_EQUAL(0, testOutput.GetMaxDepth());
	}


	TEST(No_Calls_TestOutput)
	{
		SuperProfiler::Root::Reset();

		//Create a default test output object
		SuperProfiler::TestOutput testOutput;
		SuperProfiler::Root::OutputResults(testOutput);

		//Nothing written to the output, so it's values should be at default
		CHECK_EQUAL(0, testOutput.GetMaxDepth());
	}


	TEST(One_Call_TestOutput)
	{
		SuperProfiler::Root::Reset();

		TestFunctionCall1();

		//Create a default test output object
		SuperProfiler::TestOutput testOutput;
		SuperProfiler::Root::OutputResults(testOutput);

		//One function call
		CHECK_EQUAL(1, testOutput.GetMaxDepth());
	}


	//NOTE: Because it deals with time, this test is only expected to work in release mode
	TEST(Complex_TestOutput)
	{
		SuperProfiler::Root::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		//Create a default test output object
		SuperProfiler::TestOutput testOutput;
		SuperProfiler::Root::OutputResults(testOutput);

		//Make sure the total run time is about what we expect
		CHECK_CLOSE(1.35, testOutput.GetTotalRunTime(), 0.05);

		//Make sure MAIN() used 100% of the time
		CHECK_EQUAL(testOutput.GetTotalRunTime(), testOutput.GetFunctionRunTime("MAIN()"));

		//Make sure TestFunctionCall4() was called twice
		CHECK_EQUAL(2, testOutput.GetFunctionCallAmount("void TestFunctionCall4(void)"));
	}


	TEST(TextOutput_Invalid_Results)
	{
		SuperProfiler::Root::Reset();

		//Invalid profile since it is in the same scope as the SuperProfiler::Root::OutputResults() call.
		SUPER_PROFILE("MAIN()");

		TestFunctionCall4();

		//Create a default TextOutput object
		SuperProfiler::TextOutput textOutput("SuperProfilerInvalidResults.txt");
		//This should fail since the above MAIN() profile is in the same scope as us
		CHECK_EQUAL(false, SuperProfiler::Root::OutputResults(textOutput));

		//File should be empty
		std::ifstream ifs("SuperProfilerInvalidResults.txt", std::ifstream::in);
		ifs.seekg(0, std::ios::end);
		int fileLength = ifs.tellg();
		CHECK_EQUAL(0, fileLength);
	}


	TEST(TextOutput_Valid_Results)
	{
		SuperProfiler::Root::Reset();

		{
			SUPER_PROFILE("MAIN()");
			{
				TestFunctionCall4();
				TestFunctionCall4();
			}
		}

		//Create a default TextOutput object
		SuperProfiler::TextOutput textOutput("SuperProfilerValidResults.txt");
		CHECK_EQUAL(true, SuperProfiler::Root::OutputResults(textOutput));
	}


	TEST(Recursion_Function_Call)
	{
		SuperProfiler::Root::Reset();

		int i = 0;
		TestFunctionCallRecursion1(i);

		//Create a default test output object
		SuperProfiler::TestOutput testOutput;
		SuperProfiler::Root::OutputResults(testOutput);

		CHECK_EQUAL(5, testOutput.GetMaxDepth());
	}
}


int main(int, char const * [])
{
	return UnitTest::RunAllTests();
}