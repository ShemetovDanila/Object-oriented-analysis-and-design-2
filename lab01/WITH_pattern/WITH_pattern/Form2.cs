using System.Windows.Forms;

namespace WITH_pattern
{
    public partial class Form2 : Form
    {
        private Form1 _main;
        private ListBox lb;
        public Form2(Form1 main)
        {
            _main = main; this.Size = new System.Drawing.Size(300, 400);
            lb = new ListBox { Dock = DockStyle.Fill };
            this.Controls.Add(lb);
            foreach (var h in _main.GetHistory()) lb.Items.Add(h);
        }
    }
}