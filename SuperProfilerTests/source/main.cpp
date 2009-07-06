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
#include "SuperException.h"
#include "SuperUtils.h"
#include "SuperOutputTest.h"
#include "SuperOutputText.h"
#include "SuperOutputCSV.h"
#include "SuperOutputXML.h"
#include "SuperProfilerNet.h"
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


	void PushAndPopProfileFunction(void)
	{
		char * funcName = "PushAndPopProfileFunction()";
		SUPER_PROFILE_PUSH(funcName);

		UnitTest::TimeHelpers::SleepMs(10);

		SUPER_PROFILE_POP(funcName);
	}


	void InvalidPushAndPopProfileFunction(void)
	{
		SUPER_PROFILE_PUSH("PushAndPopProfileFunction()");

		UnitTest::TimeHelpers::SleepMs(10);

		SUPER_PROFILE_POP("AnotherPushAndPopProfileFunction()");
	}


	//This is to test the SuperStack without SuperRoot
	TEST(SuperStack_Alone)
	{
		SuperProfiler::SuperTimer testTimer;
		SuperProfiler::SuperStack testStack;

		//First create some test functions
		SuperProfiler::SuperFunctionData mainLoop(1, "MainLoop()");
		SuperProfiler::SuperFunctionData apple(2, "Apple()");
		SuperProfiler::SuperFunctionData orange(3, "Orange()");
		SuperProfiler::SuperFunctionData peach(4, "Peach()");

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
				testStack.Pop(&orange, testTimer.GetTimeSeconds());
				//Child 1 again
				testStack.Push(&orange, testTimer.GetTimeSeconds());
				{
					//Child 1
					testStack.Push(&peach, testTimer.GetTimeSeconds());
					{
					}
					testStack.Pop(&peach, testTimer.GetTimeSeconds());
				}
				testStack.Pop(&orange, testTimer.GetTimeSeconds());
			}
			testStack.Pop(&apple, testTimer.GetTimeSeconds());
		}
		testStack.Pop(&mainLoop, testTimer.GetTimeSeconds());

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


	TEST(No_Main_Call_Tree)
	{
		SuperProfiler::SuperRoot::Reset();

		size_t numCalls = 5;
		for (size_t i = 0; i < numCalls; i++)
		{
			TestFunctionCall4();
		}
		int numRecCalls = 0;
		TestFunctionCallRecursion1(numRecCalls);

		//Create a default test output object
		SuperProfiler::SuperOutputTest superOutputTest;
		SuperProfiler::SuperRoot::OutputResults(superOutputTest);

		CHECK_EQUAL(numCalls, superOutputTest.GetFunctionCallAmount("TestFunctionCall4()"));
		CHECK_EQUAL(numCalls, superOutputTest.GetCallTree().GetFunction("TestFunctionCall4()")->GetCallAmount());

		//TestFunctionCallRecursion1() was called a total of 5 times
		CHECK_EQUAL(5, superOutputTest.GetFunctionCallAmount("TestFunctionCallRecursion1(int &)"));
		//Make sure that it was only called once per branch
		CHECK_EQUAL(1, superOutputTest.GetCallTree().GetFunction("TestFunctionCallRecursion1(int &)")->GetCallAmount());
		CHECK_EQUAL(1, superOutputTest.GetCallTree().GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->GetCallAmount());
		CHECK_EQUAL(1, superOutputTest.GetCallTree().GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->GetCallAmount());
		CHECK_EQUAL(1, superOutputTest.GetCallTree().GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->GetCallAmount());
		CHECK_EQUAL(1, superOutputTest.GetCallTree().GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->
													 GetFunction("TestFunctionCallRecursion1(int &)")->GetCallAmount());
		//Make sure the tree has the right amount of calls
		int invalidNode = (superOutputTest.GetCallTree().GetFunction("TestFunctionCallRecursion1(int &)")->
														 GetFunction("TestFunctionCallRecursion1(int &)")->
														 GetFunction("TestFunctionCallRecursion1(int &)")->
														 GetFunction("TestFunctionCallRecursion1(int &)")->
														 GetFunction("TestFunctionCallRecursion1(int &)")->
														 GetFunction("TestFunctionCallRecursion1(int &)") == NULL) ? 0 : 1;
		CHECK_EQUAL(0, invalidNode);
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
		CHECK_THROW(SuperProfiler::SuperRoot::OutputResults(superOutputText), SuperProfiler::SuperException);

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
		try
		{
			SuperProfiler::SuperRoot::OutputResults(superOutputText);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
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
		try
		{
			SuperProfiler::SuperRoot::OutputResults(csvOutput);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
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
		try
		{
			SuperProfiler::SuperRoot::OutputResults(superOutputText);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
		try
		{
			SuperProfiler::SuperRoot::OutputResults(csvOutput);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
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
		try
		{
			SuperProfiler::SuperRoot::OutputResults(superOutputText);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
		try
		{
			SuperProfiler::SuperRoot::OutputResults(csvOutput);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
		try
		{
			SuperProfiler::SuperRoot::OutputResults(xmlOutput);
		}
		catch (SuperProfiler::SuperException &)
		{
			//No exception should be thrown from OutputResults here
			CHECK(false);
		}
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


	TEST(PushAndPopProfile)
	{
		SuperProfiler::SuperRoot::Reset();

		PushAndPopProfileFunction();

		//Create a default test output object
		SuperProfiler::SuperOutputTest superOutputTest;
		SuperProfiler::SuperRoot::OutputResults(superOutputTest);

		//Make sure StartAndStopProfileFunction() used 100% of the time
		CHECK_EQUAL(superOutputTest.GetTotalRunTime(), superOutputTest.GetFunctionRunTime("PushAndPopProfileFunction()"));

		//One function call
		CHECK_EQUAL(1, superOutputTest.GetCallTree().GetFunction("PushAndPopProfileFunction()")->GetCallAmount());
	}


	TEST(InvalidPushAndPopProfile)
	{
		SuperProfiler::SuperRoot::Reset();

		CHECK_THROW(InvalidPushAndPopProfileFunction(), SuperProfiler::SuperException);
	}


	TEST(RealTimeInfo)
	{
		SuperProfiler::SuperRoot::Reset();

		TestFunctionCall4();

		SuperProfiler::SuperIterator iter = SuperProfiler::SuperRoot::GetIterator();
		CHECK(iter.IsRoot());
		CHECK_EQUAL("TestFunctionCall4()", iter.GetCurrentName());
		CHECK(iter.GetCurrentParentTotalTime() > 0);
		CHECK(iter.GetCurrentParentTotalCalls() > 0);
		CHECK_EQUAL(1, iter.GetNumChildren());
		iter.EnterChild(0);
		CHECK(!iter.IsRoot());
		CHECK_EQUAL("TestFunctionCall3()", iter.GetCurrentName());
		CHECK_EQUAL(2, iter.GetNumChildren());
		iter.EnterParent();
		CHECK(iter.IsRoot());
		CHECK_EQUAL("TestFunctionCall4()", iter.GetCurrentName());
		CHECK_EQUAL(1, iter.GetNumChildren());
	}

	TEST(RealTimeNetworkConnectionState)
	{
		SuperProfiler::SuperNetIniter initer;

		SuperProfiler::SuperRoot::Reset();

		SuperProfiler::SuperNetServer server(54321);
		SuperProfiler::SuperNetClient client;

		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());

		CHECK(client.Connect("localhost:54321"));
		CHECK_EQUAL(SuperProfiler::SuperNetClient::CONNECTING, client.GetState());

		SuperProfiler::SuperTimer timer;
		double maxWaitTime = 1;
		while (timer.GetTimeSeconds() <= maxWaitTime)
		{
			server.Process();
			client.Process();
			if (client.GetState() == SuperProfiler::SuperNetClient::CONNECTED &&
				server.GetNumClients() > 0)
			{
				break;
			}
		}

		CHECK_EQUAL(SuperProfiler::SuperNetClient::CONNECTED, client.GetState());

		CHECK_EQUAL(1, server.GetNumClients());

		CHECK(!client.Connect("localhost:54321"));
		CHECK_EQUAL(SuperProfiler::SuperNetClient::CONNECTED, client.GetState());

		client.Disconnect();
		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTING, client.GetState());

		timer.Reset();
		while (timer.GetTimeSeconds() <= maxWaitTime)
		{
			server.Process();
			client.Process();
			if (client.GetState() == SuperProfiler::SuperNetClient::DISCONNECTED &&
				server.GetNumClients() == 0)
			{
				break;
			}
		}

		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());

		CHECK_EQUAL(0, server.GetNumClients());

		client.Disconnect();
		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());
	}


	TEST(RealTimeNetworkConnectionStateNoServer)
	{
		SuperProfiler::SuperNetIniter initer;

		SuperProfiler::SuperRoot::Reset();

		SuperProfiler::SuperNetClient client;

		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());

		CHECK(client.Connect("localhost:54321"));
		CHECK_EQUAL(SuperProfiler::SuperNetClient::CONNECTING, client.GetState());

		SuperProfiler::SuperTimer timer;
		double maxWaitTime = 10;
		//Make sure the client times out
		while (true)//timer.GetTimeSeconds() <= maxWaitTime)
		{
			client.Process();
			if (client.GetState() == SuperProfiler::SuperNetClient::DISCONNECTED)
			{
				break;
			}
		}

		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());

		//Attempt to connect again
		CHECK(client.Connect("localhost:54321"));
		CHECK_EQUAL(SuperProfiler::SuperNetClient::CONNECTING, client.GetState());

		client.Disconnect();
		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTING, client.GetState());

		timer.Reset();
		//Make sure the client times out
		while (timer.GetTimeSeconds() <= maxWaitTime)
		{
			client.Process();
			if (client.GetState() == SuperProfiler::SuperNetClient::DISCONNECTED)
			{
				break;
			}
		}

		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());

		client.Disconnect();
		CHECK_EQUAL(SuperProfiler::SuperNetClient::DISCONNECTED, client.GetState());
	}


	TEST(RealTimeNetworkInfoSync)
	{
		SuperProfiler::SuperNetIniter initer;

		SuperProfiler::SuperRoot::Reset();

		//Make a call to populate the call tree
		TestFunctionCall4();

		SuperProfiler::SuperNetServer server(54321);
		SuperProfiler::SuperNetClient client;
		CHECK(client.Connect("localhost:54321"));

		SuperProfiler::SuperTimer timer;
		double maxWaitTime = 1;
		while (timer.GetTimeSeconds() <= maxWaitTime)
		{
			server.Process();
			client.Process();
			if (client.GetState() == SuperProfiler::SuperNetClient::CONNECTED &&
				server.GetNumClients() > 0)
			{
				break;
			}
		}

		CHECK_EQUAL(SuperProfiler::SuperNetClient::CONNECTED, client.GetState());
		CHECK_EQUAL(1, server.GetNumClients());

		//Make sure the tree got synchronized initially during connection from the server to client
		SuperProfiler::SuperNetIterator syncIter = client.GetIterator();
		CHECK(syncIter.IsRoot());
		CHECK_EQUAL("ROOT", syncIter.GetCurrentParentName());
		//TestFunctionCall4 was called off the root
		CHECK_EQUAL(1, syncIter.GetNumChildren());

		//Just process the server and client for a while, making sure the tree looks right
		for (size_t i = 0; i < 100; i++)
		{
			TestFunctionCall4();

			server.Process();
			client.Process();

			SuperProfiler::SuperNetIterator iter = client.GetIterator();
			CHECK(iter.IsRoot());
			CHECK_EQUAL("ROOT", iter.GetCurrentParentName());
			CHECK_EQUAL(1, iter.GetNumChildren());
			CHECK_EQUAL("TestFunctionCall4()", iter.GetCurrentName());
			iter.EnterChild(0);
			CHECK_EQUAL(2, iter.GetNumChildren());
			CHECK(!iter.IsRoot());
			CHECK_EQUAL("TestFunctionCall3()", iter.GetCurrentName());
			iter.EnterChild(0);
			CHECK_EQUAL(1, iter.GetNumChildren());
		}
	}


	TEST(SuperBitStreamSimpleTests)
	{
		SuperProfiler::SuperBitStream bitStream;

		size_t dataSize = 0;
		std::string strData("12345");
		dataSize += strData.length() + 1;
		int intData = 12345;
		dataSize += sizeof(int);
		float floatData = 12.345f;
		dataSize += sizeof(float);
		short shortData = 321;
		dataSize += sizeof(short);
		char charData = 6;
		dataSize += sizeof(char);
		bool boolData = true;
		dataSize += sizeof(bool);

		bitStream.Serialize(true, strData);
		bitStream.Serialize(true, intData);
		bitStream.Serialize(true, floatData);
		bitStream.Serialize(true, shortData);
		bitStream.Serialize(true, charData);
		bitStream.Serialize(true, boolData);

		CHECK_EQUAL(dataSize, bitStream.GetDataSize());
		CHECK(bitStream.GetData() != NULL);

		//Make sure that a read at this point will throw an exception
		bool readExcepTest = true;
		CHECK_THROW(bitStream.Serialize(false, readExcepTest), SuperProfiler::SuperException);

		//Rewind the stream so we can read from it
		bitStream.Rewind();

		std::string readStrData;
		int readIntData = 0;
		float readFloatData = 0;
		short readShortData = 0;
		char readCharData = 0;
		bool readBoolData = false;

		bitStream.Serialize(false, readStrData);
		bitStream.Serialize(false, readIntData);
		bitStream.Serialize(false, readFloatData);
		bitStream.Serialize(false, readShortData);
		bitStream.Serialize(false, readCharData);
		bitStream.Serialize(false, readBoolData);

		CHECK_EQUAL(strData, readStrData);
		CHECK_EQUAL(intData, readIntData);
		CHECK_CLOSE(floatData, readFloatData, 0.001f);
		CHECK_EQUAL(shortData, readShortData);
		CHECK_EQUAL(charData, readCharData);
		CHECK_EQUAL(boolData, readBoolData);

		//Start over and try again with a different set of data
		bitStream.Clear();

		CHECK_EQUAL(0, bitStream.GetDataSize());
		CHECK(bitStream.GetData() == NULL);

		size_t dataSize2 = 0;
		std::string strData2("abcdefghijklmnopqrstuvwxyz");
		dataSize2 += strData2.length() + 1;
		int intData2 = 48151623;
		dataSize2 += sizeof(int);
		float floatData2 = 4815.1623f;
		dataSize2 += sizeof(float);
		short shortData2 = 2432;
		dataSize2 += sizeof(short);
		char charData2 = 4;
		dataSize2 += sizeof(char);
		bool boolData2 = false;
		dataSize2 += sizeof(bool);

		bitStream.Serialize(true, strData2);
		bitStream.Serialize(true, intData2);
		bitStream.Serialize(true, floatData2);
		bitStream.Serialize(true, shortData2);
		bitStream.Serialize(true, charData2);
		bitStream.Serialize(true, boolData2);

		CHECK_EQUAL(dataSize2, bitStream.GetDataSize());
		CHECK(bitStream.GetData() != NULL);

		//Make sure that a read at this point will throw an exception
		readExcepTest = true;
		CHECK_THROW(bitStream.Serialize(false, readExcepTest), SuperProfiler::SuperException);

		//Rewind the stream so we can read from it
		bitStream.Rewind();

		std::string readStrData2;
		int readIntData2 = 0;
		float readFloatData2 = 0;
		short readShortData2 = 0;
		char readCharData2 = 0;
		bool readBoolData2 = false;

		bitStream.Serialize(false, readStrData2);
		bitStream.Serialize(false, readIntData2);
		bitStream.Serialize(false, readFloatData2);
		bitStream.Serialize(false, readShortData2);
		bitStream.Serialize(false, readCharData2);
		bitStream.Serialize(false, readBoolData2);

		CHECK_EQUAL(strData2, readStrData2);
		CHECK_EQUAL(intData2, readIntData2);
		CHECK_CLOSE(floatData2, readFloatData2, 0.001f);
		CHECK_EQUAL(shortData2, readShortData2);
		CHECK_EQUAL(charData2, readCharData2);
		CHECK_EQUAL(boolData2, readBoolData2);
	}
}


int main(int, char const * [])
{
	return UnitTest::RunAllTests();
}