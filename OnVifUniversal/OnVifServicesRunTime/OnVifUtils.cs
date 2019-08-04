using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Popups;
using OnVifServices;

namespace OnVifServicesRunTime
{
    public sealed class DividedByCalculator
    {

        public static long getNumberDividedBy(long Number, long BaseValue)
        {
            while (true)
            {
                if ((Number % BaseValue) == 0) // modulo for integer divided
                {
                    return Number;
                }
                else Number++;
            }

        }
    }
    /*
    public sealed class commandUI 
    {
        private string m_Label;
        private int m_ID;

        public string Label
        {
            get { return this.m_Label; }
        }
        public int ID
        {
            get { return this.m_ID; }
        }


        public commandUI(string label,  int ID)
        {
            this.m_Label = label;
            this.m_ID = ID;
        }

    }
    */
    internal sealed class OnVifMsgbox
    {
  //      internal Windows.Foundation.TypedEventHandler<Object, commandUI> UICommandHandler=null;
        //     private IList<UICommand> m_UICommands;

        private string m_msgTitle;
        private string m_mytext;
        public OnVifMsgbox(OnVifException ex)
        {
             m_msgTitle = ex.Message;
             m_mytext = ex.InnerException.Message;

        }
 
        public async void Show()
        {


            var dialog = new MessageDialog(m_mytext, m_msgTitle);   
            dialog.Commands.Clear();
     
            // Set the command to be invoked when a user presses 'ESC'
            dialog.CancelCommandIndex = 0;

   //         dialog.Commands.Add(new UICommand { Label = "Yes",      Id = 0, Invoked = new UICommandInvokedHandler(CommandInvokedHandler) });
   //         dialog.Commands.Add(new UICommand { Label = "No",       Id = 1, Invoked = new UICommandInvokedHandler(CommandInvokedHandler) });
   //         dialog.Commands.Add(new UICommand { Label = "Close Application",   Id = 2, Invoked = new UICommandInvokedHandler(CommandInvokedHandler) });

            var res = await dialog.ShowAsync();

       
        }


        private void CommandInvokedHandler(IUICommand command)
        {
         
            if ((int) command.Id == 0) // Yes
            {


            }
            if ((int)command.Id == 1) // No
            {

            }
            if ((int)command.Id == 2) // Close Application
            {
                Windows.UI.Xaml.Application.Current.Exit();
            }
         //   OnCommandUIChange(command);
            // Do something here on the command handler
        }
    }

}
