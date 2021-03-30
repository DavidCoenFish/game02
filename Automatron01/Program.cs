namespace Automatron01
{
   class Program
   {
      [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
      public static extern bool PostMessage(System.IntPtr hWnd, int Msg, System.IntPtr wParam, System.IntPtr lParam);
      const int WM_SYSKEYDOWN = 0x0104;
      const int VK_RETURN = 0x0D;
      const long KEY_STATE_DOWN = 0x20000000;
      //const long KEY_STATE_DOWN_STATE = 0x40000000;
      //const long KEY_STATE_MASK = KEY_STATE_DOWN  | KEY_STATE_DOWN_STATE;
      const int WM_CLOSE = 0x0010;

      private static void OutputHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.WriteLine(data.Data);
      }
      private static void ErrorHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.Error.WriteLine(data.Data);
      }

      static void ProcessStart(string exePath, string armuments, params System.Action<System.Diagnostics.Process>[] actionList)
      {
         System.Console.WriteLine(System.String.Format("ProcessStart: {0} {1}", exePath, armuments));

         var process = new System.Diagnostics.Process();
         process.StartInfo = new System.Diagnostics.ProcessStartInfo(exePath);
         //process.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Minimized;
         //process.StartInfo.Arguments = "www.northwindtraders.com";
         process.StartInfo.UseShellExecute = false;
         process.StartInfo.ErrorDialog = false;
         //process.StartInfo.RedirectStandardInput = true;
         process.StartInfo.RedirectStandardOutput = true;
         process.StartInfo.RedirectStandardError = true;
         process.StartInfo.Arguments = armuments;
         process.StartInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(exePath);

         process.OutputDataReceived += OutputHandler;
         process.ErrorDataReceived += ErrorHandler;

#if true
         if (false == process.Start())
         {
            System.Console.Error.WriteLine(System.String.Format("Could not start process:{0}", exePath));
            System.Environment.Exit(-1);
         }
         process.BeginOutputReadLine();
         process.BeginErrorReadLine();

         for (int index = 0; index < actionList.Length; index++)
         {
            actionList[index](process);
         }
         process.WaitForExit(100000);
         process.Close();
#elif false
         if (true == process.Start())
         {
            // read chunk-wise while process is running.
            while (!process.HasExited)
            {
               var output = process.StandardOutput.ReadToEnd();
               if (0 < output.Length)
               {
                  System.Console.WriteLine(output);
               }
               var error = process.StandardError.ReadToEnd();
               if (0 < error.Length)
               {
                  System.Console.WriteLine(error);
               }
            }
            process.Close();
         }
         else
         {
            System.Console.Error.WriteLine(System.String.Format("Could not start process:{0}", exePath));
            System.Environment.Exit(-1);
         }
#else
         // make sure not to miss out on any remaindings.
         standardOutput.Append(process.StandardOutput.ReadToEnd());
         process.OutputDataReceived += (sender, e) => { System.Console.WriteLine(e.Data); };
         process.ErrorDataReceived += (sender, e) => { System.Console.Error.WriteLine(e.Data); };
         if (false == process.Start())
         {
            System.Console.Error.WriteLine(System.String.Format("Could not start process:{0}", exePath));
            System.Environment.Exit(-1);
         }

         process.BeginOutputReadLine();
         process.BeginErrorReadLine();
         for (int index = 0; index < actionList.Length; index++)
         {
            actionList[index](process);
         }
         process.WaitForExit(100000);
         process.Close();
#endif
         System.Console.WriteLine("ProcessEnd");
      }

      static System.Action<System.Diagnostics.Process> ActionFactoryWait(int wait)
      {
         return (System.Diagnostics.Process process) =>
         {
            System.Threading.Thread.Sleep(wait);
         };
      }

      static System.Action<System.Diagnostics.Process> ActionFactoryPostMessage(int Msg, System.IntPtr wParam, System.IntPtr lParam)
      {
         return (System.Diagnostics.Process process) =>
         {
            PostMessage(process.MainWindowHandle, Msg, wParam, lParam);
         };
      }

      static void Main(string[] args)
      {
         System.Console.WriteLine("Main start");

         //ProcessStart("C:\\Windows\\Microsoft.NET\\Framework\\v4.0.30319\\MSBuild.exe", "G:\\dcoen\\game02\\Game02.sln /t:Build /p:Configuration=Release /p:Platform=Gaming.Desktop.x64 /verbosity:diag");
         //C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Microsoft\VC\v160\Microsoft.Cpp.Default.props ?
         //string solutionConfiguration = @"Release";
         string solutionConfiguration = @"Debug";

         /*
         ProcessStart(
            @"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com",
            @"G:\dcoen\game02\Game02.sln /clean """ + solutionConfiguration + @"|Gaming.Desktop.x64"""
            );
         */
         /*
         ProcessStart(
            @"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.com",
            @"G:\dcoen\game02\Game02.sln /build """ + solutionConfiguration + @"|Gaming.Desktop.x64"" /project Application00"
            );
         */

         var cmdParamArray = new string[] {
            //@"Resources\DAG\SimpleTriangleApplication.json"
            //@"Resources\DAG\DepthTriangleApplication.json"
            @"Resources\DAG\SphereGrid00Application.json"
            //@""
            };

         for (int index = 0; index < cmdParamArray.Length; index++)
         {
            ProcessStart(
               @"G:\dcoen\Game02Build\Application00\Gaming.Desktop.x64\" + solutionConfiguration + @"\output\Application00.exe",
               cmdParamArray[index],
               ActionFactoryWait(100000),
               ActionFactoryWait(1000),
               ActionFactoryPostMessage(WM_SYSKEYDOWN, new System.IntPtr(VK_RETURN), new System.IntPtr(KEY_STATE_DOWN)),
               ActionFactoryWait(1000),
               ActionFactoryPostMessage(WM_SYSKEYDOWN, new System.IntPtr(VK_RETURN), new System.IntPtr(KEY_STATE_DOWN)),
               ActionFactoryWait(1000)//,
               //ActionFactoryPostMessage(WM_CLOSE, System.IntPtr.Zero, System.IntPtr.Zero)
               );
         }
      }
   }
}
