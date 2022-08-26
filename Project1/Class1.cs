using System.IO; // file class is here
using System;
namespace Project1
{
    internal class Class1
    {
        static void Main()
        {
            //open a file
            string path = @"D:\Oasis\src\AmunRa\Code\Source\Objects\Oasis_PCSCMgr.scr";
            string wPath = @"out.scr";
            string l;
            if (!File.Exists(path))
            {
                Console.WriteLine("File not found");
                return;
            }

            /*create a output file*/

            File.Create(wPath).Close();

            //Output file to open ins stream
            StreamWriter wfile = new StreamWriter(wPath);

            //Read all lines
            var lines = File.ReadAllLines(path);

            foreach (var line in lines)
            {
                l = line;
                //check line must not be started with comment
                if (!l.StartsWith(@"//"))
                {
                    //if its not a commented line
                    //append ; in the end of line
                    l = line.Insert(line.Length,";");

                }

                //Add line in output file
                wfile.WriteLine(l);

            }

            //Close the output stream
            wfile.Close();

            //Delete the input script file
            File.Delete(path);

            //create a new file and copy the content of output script file
            File.Copy(wPath, path);

            //Delete temp file
            File.Delete(wPath);

        }
    }
}
