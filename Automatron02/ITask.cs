namespace Automatron02
{
   interface ITask
   {
      int Priority
      {
         get;
      }
      string Name
      {
         get;
      }
      string[] Dependants
      {
         get;
      }
      bool Skip
      {
         get;
      }
      bool HasRun
      {
         get;
         set;
      }

      //void Run(System.Collections.Generic.Dictionary<string, ITask> taskCollection);
      bool Run(System.Collections.Generic.List<ITask> taskDependants);
      //void Run();
   }
}
