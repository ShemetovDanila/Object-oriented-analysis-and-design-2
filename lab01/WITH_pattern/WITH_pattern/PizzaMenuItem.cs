using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Linq;

namespace WITH_pattern
{
    public class PizzaMenuItem
    {
        public string Name { get; set; }
        public string ImagePath { get; set; }
        public Pizza Template { get; set; }
        public bool IsCustom { get; set; }
        public Panel Panel { get; set; }
        public Dictionary<string, NumericUpDown> ToppingControls { get; set; } = new Dictionary<string, NumericUpDown>();
        private ComboBox cbDough;
        private ComboBox cbSauce;

        public PizzaMenuItem(string name, int price, string imagePath, Pizza template, bool isCustom)
        {
            this.Name = name; this.ImagePath = imagePath; this.Template = template; this.IsCustom = isCustom;
        }

        public Panel CreatePanel(EventHandler onAddClick)
        {
            Panel = new Panel { Size = new Size(880, 340), BackColor = Color.White, Margin = new Padding(0, 0, 0, 25) };
            Panel.Paint += (s, e) => ControlPaint.DrawBorder(e.Graphics, Panel.ClientRectangle, Color.FromArgb(220, 220, 220), ButtonBorderStyle.Solid);

            PictureBox pb = new PictureBox { Size = new Size(220, 220), Location = new Point(20, 20), SizeMode = PictureBoxSizeMode.Zoom };
            try { pb.Image = Image.FromFile(System.IO.Path.Combine(Application.StartupPath, "img", ImagePath)); } catch { pb.BackColor = Color.FromArgb(245, 245, 245); }
            Panel.Controls.Add(pb);

            Label lblName = new Label { Text = Name, Font = new Font("Segoe UI", 22, FontStyle.Bold), Location = new Point(260, 15), AutoSize = true };
            Panel.Controls.Add(lblName);

            int curY = 65;
            if (IsCustom)
            {
                cbDough = new ComboBox { Location = new Point(260, curY), Width = 200, DropDownStyle = ComboBoxStyle.DropDownList, Font = new Font("Segoe UI", 9) };
                foreach (var d in ToppingCatalog.Doughs) cbDough.Items.Add($"{d.name} (+{d.price}₽)");
                cbDough.SelectedIndex = 0;

                cbSauce = new ComboBox { Location = new Point(480, curY), Width = 200, DropDownStyle = ComboBoxStyle.DropDownList, Font = new Font("Segoe UI", 9) };
                foreach (var s in ToppingCatalog.Sauces) cbSauce.Items.Add($"{s.name} (+{s.price}₽)");
                cbSauce.SelectedIndex = 0;
                Panel.Controls.Add(cbDough); Panel.Controls.Add(cbSauce);
                curY += 50;
            }

            // Равномерная сетка топпингов
            int tx = 260, ty = curY;
            int colWidth = 200; // Увеличил ширину колонки
            int rowHeight = 35; // Увеличил высоту строки

            foreach (var t in ToppingCatalog.Toppings)
            {
                Label l = new Label { Text = $"{t.name} ({t.price}₽)", Location = new Point(tx, ty + 3), Width = 140, Font = new Font("Segoe UI", 8) };
                NumericUpDown n = new NumericUpDown { Location = new Point(tx + 145, ty), Width = 45, Minimum = 0, Font = new Font("Segoe UI", 9) };
                ToppingControls[t.name] = n;
                Panel.Controls.Add(l); Panel.Controls.Add(n);

                ty += rowHeight;
                if (ty > 300) { ty = curY; tx += colWidth; }
            }

            Button btn = new Button
            {
                Text = "ДОБАВИТЬ В КОРЗИНУ",
                Size = new Size(220, 55),
                Location = new Point(20, 260),
                BackColor = Color.FromArgb(255, 140, 0),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Segoe UI", 11, FontStyle.Bold),
                Cursor = Cursors.Hand
            };
            btn.FlatAppearance.BorderSize = 0;
            btn.Click += onAddClick;
            Panel.Controls.Add(btn);

            return Panel;
        }

        public Pizza CreatePizzaWithToppings()
        {
            PizzaBuilder b = new PizzaBuilder();
            b.Reset().SetName(Name).SetBasePrice(Template.price).SetIsCustom(IsCustom);

            if (IsCustom)
            {
                b.SetDough(cbDough.Text.Split('(')[0].Trim()).SetSauce(cbSauce.Text.Split('(')[0].Trim());
            }
            else
            {
                b.SetDough(Template.Dough.name).SetSauce(Template.Sauce.name);
                foreach (var t in Template.Toppings) b.AddTopping(t.name);
            }

            foreach (var ctrl in ToppingControls)
            {
                for (int i = 0; i < (int)ctrl.Value.Value; i++) b.AddTopping(ctrl.Key);
            }
            return b.GetResult();
        }

        public List<string> GetUserSelectedExtras()
        {
            return ToppingControls.Where(c => c.Value.Value > 0)
                                 .Select(c => $"{c.Key} x{c.Value.Value}").ToList();
        }
    }
}