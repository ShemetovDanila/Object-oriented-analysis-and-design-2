using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

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

        public PizzaMenuItem(string name, int price, string imagePath, Pizza template, bool isCustom)
        {
            this.Name = name;
            this.ImagePath = imagePath;
            this.Template = template;
            this.IsCustom = isCustom;
        }

        public Panel CreatePanel(EventHandler onAddClick)
        {
            Panel = new Panel { Size = new Size(820, 240), BackColor = Color.White, Margin = new Padding(0, 0, 0, 15) };

            PictureBox pb = new PictureBox
            {
                Size = new Size(180, 180),
                Location = new Point(10, 10),
                SizeMode = PictureBoxSizeMode.Zoom
            };
            try { pb.Image = Image.FromFile("img/" + ImagePath); } catch { pb.BackColor = Color.FromArgb(245, 245, 245); }
            Panel.Controls.Add(pb);

            Label lblName = new Label { Text = Name, Font = new Font("Arial", 16, FontStyle.Bold), Location = new Point(200, 10), AutoSize = true };
            Panel.Controls.Add(lblName);

            int x = 200, y = 45;
            foreach (var t in ToppingCatalog.Toppings)
            {
                Label l = new Label { Text = t.name, Location = new Point(x, y), Width = 90, Font = new Font("Arial", 8) };
                NumericUpDown n = new NumericUpDown { Location = new Point(x + 90, y), Width = 40, Minimum = 0, Maximum = 10 };
                ToppingControls[t.name] = n;
                Panel.Controls.Add(l); Panel.Controls.Add(n);
                y += 28; if (y > 210) { y = 45; x += 145; }
            }

            Button btn = new Button
            {
                Text = "В КОРЗИНУ",
                Size = new Size(120, 40),
                Location = new Point(680, 10),
                BackColor = Color.FromArgb(255, 140, 0),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Arial", 9, FontStyle.Bold)
            };
            btn.FlatAppearance.BorderSize = 0;
            btn.Click += onAddClick;
            Panel.Controls.Add(btn);

            return Panel;
        }

        public Pizza CreatePizzaWithToppings()
        {
            PizzaBuilder builder = new PizzaBuilder();
            builder.Reset()
                   .SetName(this.Name)
                   .SetBasePrice(Template.price)
                   .SetIsCustom(this.IsCustom)
                   .SetDough(Template.Dough.name)
                   .SetSauce(Template.Sauce.name);

            foreach (var t in Template.Toppings)
            {
                builder.AddTopping(t.name);
            }

            foreach (var ctrl in ToppingControls)
            {
                for (int i = 0; i < (int)ctrl.Value.Value; i++)
                {
                    builder.AddTopping(ctrl.Key);
                }
            }
            return builder.GetResult();
        }
    }
}