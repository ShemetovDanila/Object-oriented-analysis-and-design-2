using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Linq;
using System.IO;

namespace WITH_pattern
{
    public class PizzaMenuItem
    {
        public string Name { get; set; }
        public Pizza Template { get; set; }
        public bool IsCustom { get; set; }
        public string ImagePath { get; set; }
        public Panel Panel { get; private set; }
        public Dictionary<string, NumericUpDown> ToppingControls { get; } = new Dictionary<string, NumericUpDown>();
        private ComboBox cbDough, cbSauce;

        public PizzaMenuItem(string name, int price, string image, Pizza template, bool custom)
        {
            Name = name; ImagePath = image; Template = template; IsCustom = custom;
        }

        public Panel CreatePanel(EventHandler onAdd)
        {
            Panel = new Panel { Size = new Size(880, 360), BackColor = Color.White, Margin = new Padding(0, 0, 0, 25), BorderStyle = BorderStyle.FixedSingle };

            PictureBox pic = new PictureBox
            {
                Size = new Size(220, 220),
                Location = new Point(20, 20),
                SizeMode = PictureBoxSizeMode.Zoom
            };

            if (File.Exists(ImagePath))
                pic.Image = Image.FromFile(ImagePath);
            else
                pic.BackColor = Color.LightGray;

            Panel.Controls.Add(pic);

            Label lblName = new Label { Text = Name, Font = new Font("Segoe UI", 20, FontStyle.Bold), Location = new Point(260, 15), AutoSize = true };
            Panel.Controls.Add(lblName);

            Label lblBasePrice = new Label { Text = $"базовая цена: {Template.price} ₽", Font = new Font("Segoe UI", 10), ForeColor = Color.DarkGray, Location = new Point(lblName.Right + 15, 26), AutoSize = true };
            Panel.Controls.Add(lblBasePrice);

            int curY = 70;
            if (IsCustom)
            {
                cbDough = new ComboBox { Location = new Point(260, curY), Width = 200, DropDownStyle = ComboBoxStyle.DropDownList };
                foreach (var d in ToppingCatalog.Doughs) cbDough.Items.Add($"{d.name} (+{d.price}₽)");
                cbDough.SelectedIndex = 0;

                cbSauce = new ComboBox { Location = new Point(480, curY), Width = 200, DropDownStyle = ComboBoxStyle.DropDownList };
                foreach (var s in ToppingCatalog.Sauces) cbSauce.Items.Add($"{s.name} (+{s.price}₽)");
                cbSauce.SelectedIndex = 0;

                Panel.Controls.Add(cbDough);
                Panel.Controls.Add(cbSauce);
                curY += 45;
            }

            int tx = 260, ty = curY;
            foreach (var t in ToppingCatalog.Toppings)
            {
                Label l = new Label { Text = $"{t.name} ({t.price}₽)", Location = new Point(tx, ty + 3), Width = 140, Font = new Font("Segoe UI", 8) };
                NumericUpDown n = new NumericUpDown { Location = new Point(tx + 145, ty), Width = 45, Maximum = 10 };
                ToppingControls[t.name] = n;
                Panel.Controls.Add(l);
                Panel.Controls.Add(n);
                ty += 42;
                if (ty > 320) { ty = curY; tx += 200; }
            }

            Button btn = new Button
            {
                Text = "В КОРЗИНУ",
                Size = new Size(220, 60),
                Location = new Point(20, 270),
                BackColor = Color.FromArgb(255, 140, 0),
                ForeColor = Color.White,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Segoe UI", 10, FontStyle.Bold)
            };
            btn.Click += onAdd;
            Panel.Controls.Add(btn);

            return Panel;
        }

        public Pizza CreatePizzaWithToppings()
        {
            PizzaBuilder b = new PizzaBuilder();
            b.Reset().SetName(Name).SetBasePrice(Template.price).SetIsCustom(IsCustom);

            if (IsCustom && cbDough != null && cbSauce != null)
            {
                var dough = ToppingCatalog.Doughs[cbDough.SelectedIndex];
                var sauce = ToppingCatalog.Sauces[cbSauce.SelectedIndex];

                b.SetDough(dough.name)
                 .SetSauce(sauce.name)
                 .SetBasePrice(Template.price + dough.price + sauce.price);
            }
            else
            {
                b.SetDough(Template.Dough.name)
                 .SetSauce(Template.Sauce.name);

                foreach (var t in Template.Toppings)
                    b.AddTopping(t.name);
            }

            foreach (var kvp in ToppingControls)
                for (int i = 0; i < (int)kvp.Value.Value; i++)
                    b.AddTopping(kvp.Key);

            return b.GetResult();
        }

        public List<string> GetUserSelectedExtras()
        {
            return ToppingControls
                .Where(x => x.Value.Value > 0)
                .OrderBy(x => x.Key)
                .Select(x => $"{x.Key} x{x.Value.Value}")
                .ToList();
        }

        public void ResetSelection()
        {
            foreach (var n in ToppingControls.Values)
                n.Value = 0;

            if (IsCustom && cbDough != null)
            {
                cbDough.SelectedIndex = 0;
                cbSauce.SelectedIndex = 0;
            }
        }
    }
}