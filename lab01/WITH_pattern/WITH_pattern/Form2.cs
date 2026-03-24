using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;

namespace WITH_pattern
{
    public partial class Form2 : Form
    {
        public Form2(List<string> history)
        {
            this.Text = "Архив ваших заказов";
            this.Size = new Size(500, 700);
            this.StartPosition = FormStartPosition.CenterParent;
            this.BackColor = Color.White;

            TextBox tb = new TextBox
            {
                Dock = DockStyle.Fill,
                Multiline = true,
                ReadOnly = true,
                ScrollBars = ScrollBars.Vertical,
                Font = new Font("Consolas", 10),
                BorderStyle = BorderStyle.None,
                Margin = new Padding(10)
            };
            tb.Text = history.Count > 0 ? string.Join("\r\n", history) : "История пока пуста...";
            this.Controls.Add(tb);
        }
    }
}