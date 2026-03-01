using System.Windows.Forms;

namespace WITH_pattern
{
    public partial class Form2 : Form
    {
        private Form1 mainForm;
        private ListBox listBoxHistory;

        public Form2(Form1 owner)
        {
            mainForm = owner;
            this.Text = "История заказов";
            this.Size = new System.Drawing.Size(400, 600);
            listBoxHistory = new ListBox
            {
                Dock = DockStyle.Fill,
                Font = new System.Drawing.Font("Arial", 10)
            };
            this.Controls.Add(listBoxHistory);
        }

        public void AddOrderToHistory(string info)
        {
            listBoxHistory.Items.Add(info);
        }
    }
}