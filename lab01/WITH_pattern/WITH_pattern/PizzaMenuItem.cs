using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace WITH_pattern
{
    public class PizzaMenuItem
    {
        public string Name { get; set; }
        public Pizza Template { get; set; }
        public Panel Panel { get; set; }
        public Dictionary<string, NumericUpDown> ToppingControls { get; set; } = new Dictionary<string, NumericUpDown>();

        public PizzaMenuItem(string name, int price, Pizza template)
        {
            this.Name = name;
            this.Template = template;
        }

        public Panel CreatePanel(EventHandler onAddClick)
        {
            Panel = new Panel { Size = new Size(800, 260), BorderStyle = BorderStyle.FixedSingle, BackColor = Color.White, Margin = new Padding(10) };

            Label lblName = new Label { Text = Name, Font = new Font("Arial", 16, FontStyle.Bold), Location = new Point(20, 10), AutoSize = true };
            Panel.Controls.Add(lblName);

            int x = 20, y = 50;
            foreach (var t in ToppingCatalog.Toppings)
            {
                Label l = new Label { Text = t.name, Location = new Point(x, y), Width = 100 };
                NumericUpDown n = new NumericUpDown { Location = new Point(x + 105, y), Width = 40, Minimum = 0, Maximum = 10 };
                ToppingControls[t.name] = n;
                Panel.Controls.Add(l);
                Panel.Controls.Add(n);

                y += 30;
                if (y > 210) { y = 50; x += 160; }
            }

            Button btn = new Button
            {
                Text = "Добавить в заказ",
                Size = new Size(150, 40),
                Location = new Point(630, 10),
                BackColor = Color.FromArgb(255, 140, 0),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat
            };
            btn.Click += onAddClick;
            Panel.Controls.Add(btn);

            return Panel;
        }

        public Pizza CreatePizza(IPizzaBuilder builder)
        {
            builder.Reset()
                   .SetName(this.Name)
                   .SetBasePrice(Template.price)
                   .SetIsCustom(Template.custom)
                   .SetDough(Template.Dough.name)
                   .SetSauce(Template.Sauce.name);

            // Сначала добавляем стандартные топпинги из шаблона директора
            foreach (var t in Template.Toppings)
            {
                builder.AddTopping(t.name);
            }

            // Затем добавляем дополнительные топпинги, выбранные пользователем в NumericUpDown
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