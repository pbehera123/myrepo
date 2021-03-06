// VoteR2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "TemplatedArray.h"
#include "NvraRecord.h"
#include "Exceptions.h"
#include "Search.h"
#include "Sorter.h"
#include "Comparator.h"
#include "NvraComparator.h"
#include "OULink.h"
#include "OULinkedList.h"
#include "Enumerator.h"
#include "OULinkedListEnumerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

//All functions/methods that will be used are described here. There are two functions for the two loops
void dataProcessLoop(bool &isValid , string fileName, OULinkedList<NvraRecord>* myList , int& linesRead, int& recordsRead, int& recordNumber, int& recordsInMemory);
void dataManipulateLoop(OULinkedList<NvraRecord>* list , bool flag, int& linesRead, int& recordsRead, int& recordsInMemory);
TemplatedArray<NvraRecord>* addToArray(OULinkedList<NvraRecord>* list);

//Main function that stores the code to be run
int main()
{

	//Setting counter variables and initializing space in arrays to be used
	int linesRead = 0;
	int recordsRead = 0;
	int recordsInMemory = 0; 
	int recordNumber = 0;
	bool isFile = true;
	bool valid = true;
	bool isInvalid = false;
	TemplatedArray<NvraRecord>* myArray = new TemplatedArray<NvraRecord>();
	if (myArray == NULL)
	{
		throw ExceptionMemoryNotAvailable();
	}
	NvraComparator* comparator = new NvraComparator(0);
	OULinkedList<NvraRecord>* myList = new OULinkedList<NvraRecord>(comparator);
	if (myList == NULL)
	{
		throw ExceptionMemoryNotAvailable();
	}


	cin.clear();
	cout << "Enter data file name: ";
	string fileName = "";
	getline(cin, fileName);
	if (fileName.size() == 0)
	{
		cout << "Enter data file name: ";
		string fileName = "";
		getline(cin, fileName);
		if (fileName.size() == 0)
		{
			delete myArray;
			delete myList;
			return 0;
		}

	}
	else
	{
		dataProcessLoop(valid , fileName, myList, linesRead, recordsRead, recordNumber, recordsInMemory);    //Functions using loops are called here
	}
	if (valid == false)
	{
		delete myArray;
		delete myList;
		return 0;
	}
	
	//If the array contains data after the process loop then this loop runs, otherwise the program exits
	else
	{
		dataManipulateLoop(myList, isFile, linesRead, recordsRead, recordsInMemory);
	}

	delete myArray;
	delete myList;
	return 0;

}



//This function processes the data in all files, getting file names from user input, and placing all lines
//into an array of NVRA records. It also checks for duplicates within a particular file. It takes in a boolean
//value that checks whether the user has any more files to process, the array to place records into , and 
//three counters: a counter to keep track of lines read, a counter to keep track of total records read,
//and a counter to keep track of total unique records in memory
void dataProcessLoop(bool &isValid , string fileName, OULinkedList<NvraRecord>* myList, int& linesRead, int& recordsRead, int& recordsNumber , int& recordsInMemory)
{
	int invalidCounter = 0;
	bool isInvalid = false;
	NvraComparator comparator(0);
	OULinkedListEnumerator<NvraRecord> enumerate = myList->enumerator();
	int recordNumber = 0;
	TemplatedArray<int>* invalidIDs = new TemplatedArray<int>();

	ifstream myFile(fileName);
	
		if (myFile.is_open())
		{
			string line;


			getline(myFile, line);
			int lineCount = 0;



				//Goes through each line and checks for duplicates and invalids
				while (getline(myFile, line))
				{

					stringstream ss(line);
					NvraRecord* myRecord = new NvraRecord();  //Record that will hold the line's data


					++lineCount;


					bool isInvalid = false;
					for (unsigned int i = 0; i < 24; i++)
					{

						std::string sub;
						if (i == 3 || i == 11 || i == 12)    //If index equals these numbers, the data is added to the strings array of the record
						{
							getline(ss, sub, ',');
							myRecord->addString(sub);

						}
						else if (i == 23)                    //Last piece of data in the line
						{
							getline(ss, sub, '\n');
							int a = atoi(sub.c_str());
							if (a >= 0)
							{

								myRecord->addNum(a);
							}
							else
							{
								
								invalidIDs->add(&lineCount);
								isInvalid = true;
								++invalidCounter;

							}

						}
						else                                          //All of the other numbers in the line
						{
							getline(ss, sub, ',');
							int a = atoi(sub.c_str());
							if (a >= 0)
							{
								myRecord->addNum(a);
							}
							else
							{
								
								invalidIDs->add(&lineCount);
								isInvalid = true;
								++invalidCounter;

							}



						}


					}
					

					//recordsRead incrementing logic suggested by Adam Gracy
					if (isInvalid == false)
					{
						++recordsRead;
					}
					recordNumber = myRecord->getNum(0);
					NvraRecord* temp = new NvraRecord();
					temp->setNum(recordNumber, 0);

					//This condition will check to see if the record has any invalid data. If it does, then 
					//the record is added to the array. If the record already matches another record in the list,
					//the error message will print and the record is not added to the array.

					if (isInvalid == false)
					{
						if (myList->getSize() == 0)
						{
							myList->append(myRecord);
						}
						else
						{
							//Enters a try-catch block, if find does not find the record in list, it throws an exception
							try
							{
								 myList->find(myRecord);
								 std::cout << "Duplicate record ID " << recordNumber << " at line " << lineCount << "." << endl;
								 continue;
								
								
							}
							catch (ExceptionLinkedListAccess*)
							{
								myList->append(myRecord);
							}
							

						}

						recordsInMemory = (int)myList->getSize();




					}
					//if useTemp is true, then the temporary array is checked for duplicates.
					




				}
				linesRead += lineCount;
				if (invalidCounter == lineCount)
				{
					cout << "No valid records found." << endl;
					cin.clear();
					cout << "Enter data file name: ";
					string fileName = "";
					getline(cin, fileName);
					if (fileName.size() == 0)
					{
						isValid = false;

					}
				}
				else if (invalidIDs->getSize() > 0)
				{
					for (unsigned long i = 0; i < invalidIDs->getSize(); ++i)
					{
						std::cout << "Invalid data at line " << invalidIDs->get(i) << "." << endl;   //Error message written out
					}
					cout << "Enter data file name: ";
					string fileName = "";
					getline(cin, fileName);
					if (fileName.size() == 0)
					{
						isValid = false;

					}
				}
		
        }
		else if (!myFile.is_open())
		{
			
			cout << "File is not available." << endl;
			cout << "Enter data file name: ";
			string fileName = "";
			getline(cin, fileName);
			if (fileName.size() == 0)
			{
				isValid = false;

			}
			
		}
}

TemplatedArray<NvraRecord>* addToArray(OULinkedList<NvraRecord>* list)
{
	OULinkedListEnumerator<NvraRecord> enumerate = list->enumerator();
	TemplatedArray<NvraRecord>* array = new TemplatedArray<NvraRecord>();
	while (enumerate.hasNext())
	{
		array->add(&(enumerate.next()));
	}
	return array;
}

//This function allows the user to carry out certain operations on an array of records: printing the records,
//sorting the records, or finding a particular record in a column. It takes in the following variables:
//the array of records to be read, a boolean flag that keeps track of whether the user wants to keep manipulating
//and three counters that are outputted along with the data: a counter to keep track of lines read, a counter to keep track of total records read,
//and a counter to keep track of total unique records in memory
void dataManipulateLoop(OULinkedList<NvraRecord>* list ,  bool flag, int& linesRead, int& recordsRead, int& recordsInMemory)
{
	unsigned int lastSorted = 0;
	NvraComparator comparator(0);
	TemplatedArray<NvraRecord>* array = addToArray(list);

	while (flag)  //SET BOOLEAN FLAG THAT ALLOWS USER TO SAY WHETHER THEY WANT TO MANIPULATE
	{

		string choice;

		cout << "Enter (o)utput, (s)ort, (f)ind, (m)erge, (p)urge, (r)ecords, or (q)uit: ";
		getline(cin, choice);
		cout << endl;
		

		//WRITE LOOP FOR IF USER INPUTS O
		//This if loop executes if user inputs o
		
			if (choice.compare("o") == 0)
			{
				string fileName;                        //String to hold name of file
				cout << "Enter output file name: ";
				getline(cin, fileName);
				ofstream myFile(fileName);              //file object to hold the name of file to be printed

				//This if loop checks if the file can be found or opened
				if (myFile.is_open())
				{


					for (unsigned int i = 0; i < array->getSize(); ++i)
					{

						try
						{

							myFile << array->get(i) << endl;                   //Prints the elements of the array to the file


						}
						catch (ExceptionIndexOutOfRange)
						{
							myFile << "Invalid index." << endl;
						}

					}
					myFile << "Data lines read: " << linesRead << "; " << "Valid NVRA records read: " << recordsRead << "; " << "NVRA records in memory: " << recordsInMemory << endl;

				}

				//This else if executes if user does not enter a file name
				else if (!myFile.is_open())
				{
					for (unsigned int i = 0; i < array->getSize(); ++i)
					{

						try
						{

							cout << array->get(i) << endl;                 //Prints the elements of the array to the standard output

						}
						catch (ExceptionIndexOutOfRange)
						{
							cout << "Invalid index." << endl;
						}
					}
					cout << "Data lines read: " << linesRead << "; " << "Valid NVRA records read: " << recordsRead << "; " << "NVRA records in memory: " << recordsInMemory << endl;
				}
			}
			else if (choice.compare("r") == 0)
			{
				string fileName;                        //String to hold name of file
				cout << "Enter output file name: ";
				getline(cin, fileName);
				ofstream myFile(fileName);              //file object to hold the name of file to be printed

				//This if loop checks if the file can be found or opened
				if (myFile.is_open())
				{
					OULinkedListEnumerator<NvraRecord>* enumerator = &list->enumerator();


					while (enumerator->hasNext())
					{

						try
						{

							myFile << enumerator->next() << endl;                   //Prints the elements of the array to the file


						}
						catch (ExceptionEnumerationBeyondEnd)
						{
							myFile << "Invalid index." << endl;
						}

					}
					myFile << "Data lines read: " << linesRead << "; " << "Valid NVRA records read: " << recordsRead << "; " << "NVRA records in memory: " << recordsInMemory << endl;

				}

				//This else if executes if user does not enter a file name
				else if (!myFile.is_open())
				{
					OULinkedListEnumerator<NvraRecord>* enumerator = &list->enumerator();


					while (enumerator->hasNext())
					{

						try
						{

							cout << enumerator->next() << endl;                 //Prints the elements of the array to the standard output

						}
						catch (ExceptionEnumerationBeyondEnd)
						{
							cout << "Invalid index." << endl;
						}
					}
					cout << "Data lines read: " << linesRead << "; " << "Valid NVRA records read: " << recordsRead << "; " << "NVRA records in memory: " << recordsInMemory << endl;
				}
			}

			//This else if executes if user inputted s, meaning user wants to sort
			else if (choice.compare("s") == 0)
			{
				
				string fieldNum = "";                            //Column to be sorted
				cout << "Enter sort field (0-23): " << endl;
				getline(cin, fieldNum);
				unsigned int num = (atoi(fieldNum.c_str()));     //Converts string to int
				if (num <= 23)
				{
					//SORTS DATA
					NvraComparator object = NvraComparator(num);
					Sorter<NvraRecord>::sort(*array, object);
					lastSorted = num;
				}

				else
				{
					break;
				}


			}

			//This else if executes if user inputted f, if user wanted to find
			else if (choice.compare("f") == 0)
			{
				
				string column = "";                                       ////Column where search is to take place
				cout << "Enter search field (0-23): " << endl;
				getline(cin, column);
				unsigned int fieldNum = atoi(column.c_str());             //Converts string to int
				unsigned int stringNum = 0;                               //Index for string array of record
				unsigned int numValue = 0;                                //Index for nums array of record
				NvraComparator object = NvraComparator(fieldNum);         //NvraComparator object set at the column given by user
				int sameRecords = 0;
				if (fieldNum <= 23)
				{
					if (fieldNum == 3 || fieldNum == 11 || fieldNum == 12)
					{
						if (fieldNum == 3)
						{
							stringNum = 0;
						}
						else if (fieldNum == 11)
						{
							stringNum = 1;
						}
						else
						{
							stringNum = 2;
						}
						NvraRecord* temp = new NvraRecord();

						string fieldValue = "";
						cout << "Enter exact text on which to search: " << endl;
						getline(cin, fieldValue);
						temp->setString(fieldValue, stringNum);

						//This condition checks if the column that is being searched has been sorted. If it has been,
						//then it will use binary search, if not then it will use linear search

						if (fieldNum != lastSorted)
						{
							for (unsigned int i = 0; i < array->getSize(); ++i)
							{
								NvraRecord input = array->get(i);
								if (object.compare(input, *temp) == 0)
								{
									cout << input << endl;
									++sameRecords;
								}
							}
							cout << "NVRA records found: " << sameRecords << "." << endl;
						}
						else
						{
							long long value = abs(binarySearch(*temp, *array, object));
							if (value >= 0)
							{
								unsigned long newValue = (unsigned long)value;


								if (object.compare(*temp, array->get(newValue)) == 0)
								{
									newValue += 1;
									++sameRecords;
									cout << array->get(newValue) << endl;
								}
								cout << "NVRA records found: " << sameRecords << "." << endl;
							}
						}
					}
					//Code to search for numeric values
					else
					{
						string column = "";
						cout << "Enter non-negative field value: " << endl;
						getline(cin, column);
						unsigned int num = (atoi(column.c_str()));
						NvraRecord* temp = new NvraRecord();
						if (fieldNum < 3)
						{
							numValue = fieldNum;
						}
						else if (fieldNum < 11)
						{
							numValue = fieldNum - 1;
						}
						else
						{
							numValue = fieldNum - 2;
						}
						temp->setNum(num, numValue);

						//This condition checks if the column that is being searched has been sorted. If it has been,
						//then it will use binary search, if not then it will use linear search
						if (fieldNum != lastSorted)
						{
							for (unsigned int i = 0; i < array->getSize(); ++i)
							{

								if (array->get(i).getNum(numValue) == num)
								{
									++sameRecords;
									cout << array->get(i) << endl;

								}
							}
							cout << "NVRA records found: " << sameRecords << "." << endl;
						}
						else
						{
							long long value = abs(binarySearch(*temp, *array, object));
							if (value >= 0)
							{
								unsigned long newValue = (unsigned long)value;

								for (long unsigned int index = newValue; index < array->getSize() - 1; ++index)
								{
									if (array->get(index).getNum(numValue) == num)
									{

										++sameRecords;
										cout << array->get(index) << endl;

									}
								}
								cout << "NVRA records found: " << sameRecords << "." << endl;
							}
						}

					}
				}
				else
				{
					break;
				}
			}
			//This executes if user inputted m, or wants to merge
			else if (choice.compare("m") == 0)
			{
				int lines = 0;
				int records = 0;
				int memoryRecords = 0;
				int number = 0;
				bool isFile = true;
				bool isValid = true;
				NvraComparator* object = new NvraComparator(0);
				OULinkedList<NvraRecord>* tempList = new OULinkedList<NvraRecord>(object);
				if (tempList == NULL)
				{
					throw ExceptionMemoryNotAvailable();
				}

				cin.clear();
				cout << "Enter data file name: ";
				string fileName = "";
				getline(cin, fileName);
				if (fileName.size() == 0)
				{
					isFile = false;

				}
				else
				{
					
					dataProcessLoop(isValid , fileName, tempList, lines, records, number, memoryRecords);
						   	
				}
				
				linesRead += lines;
				recordsRead += records;
				recordsInMemory += memoryRecords;

				//FIX ALL PARAMETERS


				
				OULinkedList<NvraRecord>* finalList = new OULinkedList<NvraRecord>(object);
				OULinkedListEnumerator<NvraRecord> enumerate = list->enumerator();
				OULinkedListEnumerator<NvraRecord> tempEnumerate = tempList->enumerator();

				while (enumerate.hasNext() && tempEnumerate.hasNext())
				{
					NvraComparator object(0);
					NvraRecord first = enumerate.peek();               
					NvraRecord second = tempEnumerate.peek();  
					

					if (object.compare(first, second) > 0)
					{
						finalList->append(&second);
						tempEnumerate.next();

					}
					else if (object.compare(first, second) == 0)
					{

						finalList->append(&second);
						tempEnumerate.next();
						enumerate.next();


					}
					else if (object.compare(first, second) < 0)
					{

						finalList->append(&first);
						enumerate.next();


					}
					

				}
				while (tempEnumerate.hasNext())
				{
					NvraRecord record = tempEnumerate.next();
					finalList->append(&record);
					
				}
				while (enumerate.hasNext())
				{
					NvraRecord record = enumerate.next();
					finalList->append(&record);
				}

				
				delete list;
				delete tempList;
				list = finalList;
				array = addToArray(list);

			}
			else if (choice.compare("p") == 0)
			{
			int lines = 0;
			int records = 0;
			int memoryRecords = 0;
			int number = 0;
			bool isFile = true;
			bool isValid = true;
			NvraComparator* object = new NvraComparator(0);
			OULinkedList<NvraRecord>* tempList = new OULinkedList<NvraRecord>(object);
			if (tempList == NULL)
			{
				throw ExceptionMemoryNotAvailable();
			}

			cin.clear();
			cout << "Enter data file name: ";
			string fileName = "";
			getline(cin, fileName);
			if (fileName.size() == 0)
			{
				isFile = false;

			}
			else
			{

				dataProcessLoop(isValid, fileName, tempList, lines, records, number, memoryRecords);

			}

			linesRead += lines;
			recordsRead += records;
			recordsInMemory += memoryRecords;

			//FIX ALL PARAMETERS



			OULinkedList<NvraRecord>* finalList = new OULinkedList<NvraRecord>(object);
			OULinkedListEnumerator<NvraRecord> enumerate = list->enumerator();
			OULinkedListEnumerator<NvraRecord> tempEnumerate = tempList->enumerator();

			while (enumerate.hasNext() && tempEnumerate.hasNext())
			{
				NvraComparator object(0);
				NvraRecord first = enumerate.peek();
				NvraRecord second = tempEnumerate.peek();

				if (object.compare(first, second) != 0)
				{
					finalList->append(&first);
					enumerate.next();
					tempEnumerate.next();
				}
				else
				{
					enumerate.next();
					tempEnumerate.next();

				}

			}
			while (tempEnumerate.hasNext())
			{
				NvraRecord record = tempEnumerate.next();
				finalList->append(&record);
			}
			while (enumerate.hasNext())
			{
				NvraRecord record = enumerate.next();
				finalList->append(&record);
			}


				delete list;
				delete tempList;
				list = finalList;
				array = addToArray(list);
			}
			//This else if executes if user inputted q, or wants to quit
			else if (choice.compare("q") == 0)
			{
				flag = false;
				cout << "Thanks for using VoteR." << endl;

			}
		
	}
}

