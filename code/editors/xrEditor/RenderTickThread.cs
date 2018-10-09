using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace xrEditor
{
    class RenderTickThread
    {
        MainWindow window;
        Thread thread;

        public RenderTickThread(MainWindow window)
        {
            this.window = window;
        }

        public void StartLoopThread()
        {
            thread = new Thread(new ThreadStart(ThreadEntry));
            thread.Start();
        }

        public void ThreadEntry()
        {
            while (true)
            {
                Thread.Sleep(33);

                window.Dispatcher.Invoke(new Action(UpdateEngine), new object[] { });
            }
        }

        public void UpdateEngine()
        {
            window.GetXRayHostInterop().UpdateEngine();
        }
    }
}
