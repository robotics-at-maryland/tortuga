/*
 * 
 * 
 * Proefixer2:   This program cleans up a Pro/E working directory for submital to a
 *               subversion repository.  It finds the most recent version of all Pro/E
 *               versioned files and renames them to the ".1" version, deleting previous
 *               versions.  It applied recursively to subdirectories.
 * 
 *               Usage:   proefixer2 [-n] [-a] [-h] [<path> [<extension> [<extension> [...]]]]
 *                        -n option:  This prevents proefixer from operating on subdirectories.
 *                        -a option:  Causes proefixer to operate on all *.<number> extension files.
 *                        -h option:  Displays this help file.
 *                        <path>:  The base directory to clean.  Default is "."
 *                        <extension>:  Filters to use when selecting files.  Default
 *                                 is the sequence "*.asm" "*.prt" "*.drw".  If any extension
 *                                 is specified, default values are not used.
 */



using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace proefixer2
{
    class Program
    {
        
        static void ProcessDirectory(bool Recursive, string path, List<string> Extensions)
        {
            //Console.WriteLine("Processing directory: " + path);

            // Delegate subdirectories

            // Get directory listing
            DirectoryInfo dir = new DirectoryInfo(path);

            if( Recursive )
                foreach (DirectoryInfo d in dir.GetDirectories())
                {
                    // Make sure we don't process subversion directories
                    if (d.Name.StartsWith("."))
                        continue;

                    // Call this program recursively for each subdirectory
                    string newpath = path + "\\" + d.Name;
                    //Console.WriteLine("Delegating directory: " + newpath);
                    ProcessDirectory(true,newpath,Extensions);

                }



            foreach (string filter in Extensions)
            {
                bool Done = false;
                while (!Done)
                {
                    Done = true;
                    foreach (FileInfo f in dir.GetFiles(filter + ".*"))
                    {
                        String name = f.Name;
                        // Look for other files with the same name
                        string maxfilename = name;
                        int maxnum = 0;
                        try
                        {
                            maxnum = Convert.ToInt32(name.Substring(name.LastIndexOf(".") + 1));
                        }
                        catch
                        {
                            continue;
                        }
                            

                        //Console.WriteLine("Looking at file \"" + name.ToString() + "\", ver " + maxnum.ToString());


                        if (dir.GetFiles(name.Substring(0, name.LastIndexOf(".")) + ".*").Length > 1 ||
                            name.EndsWith("1") == false)
                        {
                            foreach (FileInfo nf in dir.GetFiles(name.Substring(0, name.LastIndexOf(".")) + ".*"))
                            {
                                // Look at all the files with the same name
                                // Compare number
                                string thisname = nf.Name;
                                int thisnum = Convert.ToInt32(thisname.Substring(thisname.LastIndexOf(".") + 1));
                                if (thisnum >= maxnum)
                                {
                                    maxnum = thisnum;
                                    maxfilename = thisname;
                                    //Console.WriteLine("Found newer file \"" + maxfilename + "\", ver " + maxnum.ToString());
                                    if (File.Exists(path + "\\" + "~" + name + "~"))
                                        File.Delete(path + "\\" + "~" + name + "~");
                                    File.Move(path + "\\" + thisname, path + "\\~" + name + "~");

                                }
                                else
                                {
                                    //Console.WriteLine("Found older file \"" + thisname + "\", ver " + thisnum.ToString());
                                    File.Delete(path + "\\" + thisname);
                                }
                            }
                            // Copy the most recent back
                            File.Move(path + "\\" + "~" + name + "~", path + "\\" + name.Substring(0, name.LastIndexOf(".")) + ".1");
                            // Copy the most recent one
                            if(maxnum != 1)
                                Console.WriteLine("Saving file: " + path + "\\" + maxfilename + ", ver " + maxnum);
                            Done = false;
                            break;
                        }
                        else
                        {
                            // Copy the most recent one
                            //Console.WriteLine("Keeping file: " + path + "\\" + maxfilename + ", ver " + maxnum);
                        }

                    }
                }
            }

        
        }


        static void Main(string[] args)
        {
            List<string> Extensions = new List<string>();

            Console.WriteLine();
            Console.WriteLine("Pro/E File Cleaner by Tom Capon");
            
            // Check what directory we are using
            string path = ".";
            bool Recursive = true;
            bool AllExtensions = false;
            int argindex = 0;

            for (int m = 0; m < 3; m++)
            {
                if (args.Length > argindex)
                {
                    // Check for parameters
                    if (args[argindex].StartsWith("-") || args[argindex].StartsWith("/"))
                    {
                        // there is a parameter
                        if (args[argindex].ToLower().Contains("n"))
                            Recursive = false;

                        if (args[argindex].ToLower().Contains("a"))
                        {
                            Extensions.Clear();
                            Extensions.Add("*");
                            AllExtensions = true;
                        }

                        if (args[argindex].ToLower().Contains("h"))
                        {
                            // Display help data

                            Console.WriteLine();
                            Console.WriteLine("  This program cleans up a Pro/E working directory for submital to a");
                            Console.WriteLine("  subversion repository.  It finds the most recent version of all Pro/E");
                            Console.WriteLine("  versioned files and renames them to the \".1\" version, deleting previous");
                            Console.WriteLine("  versions.  By default, it processes all subdirectories as well.");
                            Console.WriteLine();
                            Console.WriteLine("  Usage:  proefixer2 [-n] [-a] [-h] [<path> [<extension> [<extension> [...]]]]");
                            Console.WriteLine("   -n :          Prevents proefixer from operating on subdirectories.");
                            Console.WriteLine("   -a :          Causes proefixer to operate on all *.<number> extension files.");
                            Console.WriteLine("                 Overrides <extension> options.");
                            Console.WriteLine("   -h :          Displays this help file.");
                            Console.WriteLine("   <path>:       The base directory to clean.  Default is the working directory.");
                            Console.WriteLine("   <extension>:  Filters to use when selecting files.  Default");
                            Console.WriteLine("                 is the sequence \"*.asm\" \"*.prt\" \"*.drw\".  If any extension");
                            Console.WriteLine("                 is specified, default values are not used.");

                            return;

                        }

                        argindex++;
                    }
                }
            }

            if (args.Length > argindex)
            {
                path = args[argindex];
                argindex++;
            }

            if (AllExtensions == false)
            {
                if (args.Length > argindex)
                {
                    Extensions.Clear();

                    for (; argindex < args.Length; argindex++)
                    {
                        // Get file extensions
                        Extensions.Add(args[argindex]);
                    }
                }
                else
                {
                    Extensions.Clear();
                    Extensions.Add("*.asm");
                    Extensions.Add("*.prt");
                    Extensions.Add("*.drw");
                }
            }

            ProcessDirectory(Recursive,path,Extensions);

            Console.WriteLine("Cleanup complete.");
            


        }
    }
}
