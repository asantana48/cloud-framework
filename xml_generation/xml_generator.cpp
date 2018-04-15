#include <iostream>
#include <fstream>
#include <string>

using namespace std;
const string TAB = "        ";


int main() 
{
	ofstream myfile;
	myfile.open("policies.xml", ios::out | ios::trunc);
	myfile << "<?xml version=\"1.0\" ?>\n<policyfile>\n";

	bool createNewPolicyList = true;
	bool createNewPolicy = true;
	while (createNewPolicyList)
	{
		myfile << TAB << "<policylist>\n";
		cout << "Created new policy list.\n\n";
		while (createNewPolicy)
		{
			int policyChoiceNum = 0;
			cout << "Available Policy Types:\n1) Size Policy\n2) Hits Policy \n3) Time Policy\n\nEnter the number of the policy type you'd like to add: ";
			cin >> policyChoiceNum;
			cout << endl;
			while (policyChoiceNum != 1 && policyChoiceNum != 2 && policyChoiceNum != 3)
			{
				cout << "Invalid choice. Try again (1, 2, or 3): ";
				cin >> policyChoiceNum;
			}
			
			//if Size Policy
			if (policyChoiceNum == 1)
			{
				cout << "Enter the name of the size policy: ";
				string sizePolicyName;
				cin >> sizePolicyName;
				cout << "Enter the lower bound for the size policy in bytes: ";
				int lowerBound;
				cin >> lowerBound;
				cout << "Enter the upper bound for the size policy in bytes: ";
				int upperBound;
				cin >> upperBound;
				cout << endl;

				myfile << TAB << TAB << "<policy type=\"sizepolicy\">\n";
				myfile << TAB << TAB << TAB << "<name>" << sizePolicyName << "</name>\n";
				myfile << TAB << TAB << TAB << "<lowerbound>" << lowerBound << "</lowerbound>\n";
				myfile << TAB << TAB << TAB << "<upperbound>" << upperBound << "</upperbound>\n";
				myfile << TAB << TAB << "</policy>\n\n";
			}
			//if hits policy
			else if (policyChoiceNum == 2)
			{
				cout << "Enter the name of the hits policy: ";
				string hitsPolicyName;
				cin >> hitsPolicyName;
				cout << "Enter the minimum number of hits for migration: ";
				int numHits;
				cin >> numHits;
				cout << endl;

				myfile << TAB << TAB << "<policy type=\"hitspolicy\">\n";
				myfile << TAB << TAB << TAB << "<name>" << hitsPolicyName << "</name>\n";
				myfile << TAB << TAB << TAB << "<minimumhits>" << numHits << "</minimumhits>\n";
				myfile << TAB << TAB << "</policy>\n\n";
			}
			//if Time Policy
			else if (policyChoiceNum == 3)
			{
				cout << "Enter the name of the time policy: ";
				string timePolicyName;
				cin >> timePolicyName;
				cout << "Enter the minimum age for migration.\n";
				cout << "Years: ";
				int years;
				cin >> years;
				cout << "Months: ";
				int months;
				cin >> months;
				cout << "Days: ";
				int days;
				cin >> days;
				cout << "Hours: ";
				int hours;
				cin >> hours;
				cout << "Minutes: ";
				int minutes;
				cin >> minutes;
				cout << "Seconds: ";
				int seconds;
				cin >> seconds;
				cout << endl;


				myfile << TAB << TAB << "<policy type=\"timepolicy\">\n";
				myfile << TAB << TAB << TAB << "<name>" << timePolicyName << "</name>\n";
				myfile << TAB << TAB << TAB << "<years>" << years << "</years>\n";
				myfile << TAB << TAB << TAB << "<months>" << months << "</months>\n";
				myfile << TAB << TAB << TAB << "<days>" << days << "</days>\n";
				myfile << TAB << TAB << TAB << "<hours>" << hours << "</hours>\n";
				myfile << TAB << TAB << TAB << "<minutes>" << minutes << "</minutes>\n";
				myfile << TAB << TAB << TAB << "<seconds>" << seconds << "</seconds>\n";
				myfile << TAB << TAB << "</policy>\n\n";

			}



			//prompts user if they want to make a new policy. If not, exits.
			char choice = '0';
			cout << "Would you add another policy to the list? (y/n): ";
			while (choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N')
			{
				cin >> choice;
				if (choice == 'y')
				{
					createNewPolicy = true;
				}
				else
				{
					createNewPolicy = false;
				}
				if (choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N')
				{
					cout << "\nInvalid input. Try again (y/n): ";
				}
			}
			cout << endl;
		}
		myfile << TAB << "</policylist>\n\n";

		//prompts user if they want to make a new policy list. If not, exits.
		char choice = '0';
		cout << "Would you like to create a new policy list? (y/n): ";
		while (choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N')
		{
			cin >> choice;
			if (choice == 'y')
			{
				createNewPolicyList = true;
				createNewPolicy = true;
			}
			else
			{
				createNewPolicyList = false;
			}
			if (choice != 'y' && choice != 'n' && choice != 'Y' && choice != 'N')
			{
				cout << "\nInvalid input. Try again (y/n): ";
			}
		}

	}




	myfile << "</policyfile>";
	myfile.close();

	cout << "\nOutput policies xml file to local directory.";

	return 0;
}

/*
<?xml version="1.0" ?>
<policyfile>
	<policylist>
		<policy type="sizepolicy">
			<name>size</name>
			<lowerbound>52</lowerbound>
			<upperbound>135</upperbound>
		</policy>

		<policy type="hitspolicy">
			<name>MyHitsPolicy</name>
			<minimumhits>24</minimumhits>
		</policy>

		<policy type="timepolicy">
			<name>MyTimePolicy</name>
			<years>1</years>
			<months>0</months>
			<days>0</days>
			<hours>0</hours>
			<minutes>0</minutes>
			<seconds>0</seconds>
		</policy>
	</policylist>

	<policylist>
		<policy type="sizepolicy">
			<name>size2</name>
			<lowerbound>20</lowerbound>
			<upperbound>180</upperbound>
		</policy>

		<policy type="hitspolicy">
			<name>hitz2</name>
			<minimumhits>24</minimumhits>
		</policy>

		<policy type="timepolicy">
			<name>timepol2</name>
			<hours>7</hours>
			<seconds>4</seconds>
		</policy>
	</policylist>

</policyfile>
*/