using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using static UvmCoreLib.UvmCoreFuncs;
using UvmCoreLib;

// This is a demo. Please remove the unused code
// and add your customized code.
namespace DemoContract1
{
    public class Storage
    {
        public string Name { get; set; }
        public int Age { get; set; }
        public string Country; // field test
        public bool IsMale { get; set; }
        public UvmArray<string> ArrayDemo { get; set; }
    }

    public class MyEventEmitteer : IUvmEventEmitter
    {
        public static void EmitHello(string eventArg)
        {
            Console.WriteLine("event Hello emited, arg is " + eventArg);
        }
        public static void EmitHello2(string eventArg)
        {
            Console.WriteLine("event Hello2 emited, arg is " + eventArg);
        }
    }

    public class MyContract : UvmContract<Storage>
    {
        public MyContract() : base(new Storage())
        {
        }
        public override void init()
        {
            print("contract initing");
            this.storage.Age = 100;
            this.storage.Country = "China";
            this.storage.Name = "C#";
            this.storage.IsMale = true;
            this.storage.ArrayDemo = UvmArray<string>.Create();
            this.storage.ArrayDemo.Add("hello");
            pprint(this);
            print("this is contract init api");
        }
        public string GetAge(string arg)
        {
            print("this is contract getAge api");
            return "" + this.storage.Age;
        }
        public string OfflineGetAge(string arg)
        {
            print("this is contract OfflineGetAge api");
            print("age is " + this.storage.Age);
            return "" + this.storage.Age;
        }
        public void TestHello(string arg)
        {
            print("this is contract hello api with argument " + arg);
        }
    }

    public class ExampleLibClass
    {
        public MyContract Main()
        {
            print("start of demo C# contract");
            var contract = new MyContract();
            print("end main");
            return contract;
        }
    }

}