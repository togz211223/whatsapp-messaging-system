#include <iostream>
using namespace std;
int main ()
{ 
   /*int n;
cout << "enter the number of lines: ";
cin >> n;
cout << endl;
int arr[n][3];
for (int i=0;i<n;i++)
{
    for(int j=0;j<3;j++)
    {
        cin >> arr[i][j];
    }
}
int sum=0;
for (int i=0;i<n;i++)
{
    for(int j=0;j<3;j++)
    {
      sum+= arr[i][j];
    }
}
if(sum==0)
    cout << "yes.";
else cout << "No."; */
int n;
cout << "enter the number of layers: ";
cin >> n;
for(int i=1;i<=n;i++)
{
    if (i%2 != 0)
        cout << " I hate";
    else cout << " I love";
    if(i!=n) cout << " that";
}
cout << " it";
return 0;
}


