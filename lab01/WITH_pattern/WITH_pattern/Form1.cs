using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace WITH_pattern
{
    public partial class Form1 : Form
    {
        private Label lblSum, lblCount;
        private FlowLayoutPanel flowOrder, flowMenu;
        private Order order = new Order();
        private List<string> history = new List<string>();
        private PizzaDirector director = new PizzaDirector();
        private Dictionary<OrderItem, List<string>> extraInfo = new Dictionary<OrderItem, List<string>>();

        private PizzaMenuItem itC, itM, itP, itK, itH, itCh;

        public Form1()
        {
            this.Text = "Pizza Master Pro";
            this.Size = new Size(1550, 950);
            this.MinimumSize = new Size(1200, 800);
            this.BackColor = Color.FromArgb(242, 242, 242);
            InitializeLayout();
        }

        private void InitializeLayout()
        {
            // Правая панель (Корзина) - ЖЕСТКАЯ СТРУКТУРА
            Panel pnlRight = new Panel { Dock = DockStyle.Right, Width = 420, BackColor = Color.White };
            this.Controls.Add(pnlRight);

            // 1. Заголовок (Top)
            Panel pnlCartHeader = new Panel { Dock = DockStyle.Top, Height = 80, BackColor = Color.FromArgb(33, 33, 33) };
            Label lblTitle = new Label { Text = "ВАШ ЗАКАЗ", Font = new Font("Segoe UI", 20, FontStyle.Bold), ForeColor = Color.White, Dock = DockStyle.Fill, TextAlign = ContentAlignment.MiddleCenter };
            pnlCartHeader.Controls.Add(lblTitle);
            pnlRight.Controls.Add(pnlCartHeader);

            // 2. Подвал (Bottom)
            Panel pnlCartFooter = new Panel { Dock = DockStyle.Bottom, Height = 260, BackColor = Color.FromArgb(250, 250, 250), Padding = new Padding(20) };
            pnlCartFooter.Paint += (s, e) => e.Graphics.DrawLine(new Pen(Color.LightGray, 1), 0, 0, 420, 0);

            lblCount = new Label { Dock = DockStyle.Top, Height = 30, Font = new Font("Segoe UI", 10), ForeColor = Color.Gray };
            lblSum = new Label { Dock = DockStyle.Top, Height = 50, Font = new Font("Segoe UI", 22, FontStyle.Bold), ForeColor = Color.FromArgb(255, 120, 0) };

            Button btnPay = new Button { Text = "ОФОРМИТЬ ЗАКАЗ", Dock = DockStyle.Top, Height = 60, BackColor = Color.FromArgb(255, 140, 0), ForeColor = Color.White, FlatStyle = FlatStyle.Flat, Font = new Font("Segoe UI", 14, FontStyle.Bold), Cursor = Cursors.Hand };
            btnPay.FlatAppearance.BorderSize = 0;
            btnPay.Click += BtnPay_Click;

            Button btnH = new Button { Text = "ИСТОРИЯ ЗАКАЗОВ", Dock = DockStyle.Bottom, Height = 40, FlatStyle = FlatStyle.Flat, ForeColor = Color.DimGray, Font = new Font("Segoe UI", 9, FontStyle.Bold) };
            btnH.Click += (s, e) => new Form2(history).Show();

            pnlCartFooter.Controls.AddRange(new Control[] { btnH, btnPay, lblSum, lblCount });
            pnlRight.Controls.Add(pnlCartFooter);

            // 3. Список (Fill) - ТЕПЕРЬ ОН ЗАЖАТ МЕЖДУ НИМИ
            flowOrder = new FlowLayoutPanel { Dock = DockStyle.Fill, AutoScroll = true, FlowDirection = FlowDirection.TopDown, WrapContents = false, Padding = new Padding(15), BackColor = Color.White };
            pnlRight.Controls.Add(flowOrder);
            flowOrder.BringToFront();

            // Левая панель (Меню)
            flowMenu = new FlowLayoutPanel { Dock = DockStyle.Fill, AutoScroll = true, Padding = new Padding(30) };
            this.Controls.Add(flowMenu);

            PizzaBuilder b = new PizzaBuilder();
            director.MakeCustomPizza(b); itC = new PizzaMenuItem("🍕 КОНСТРУКТОР", 200, "myPizza.png", b.GetResult(), true);
            director.MakeMargarita(b); itM = new PizzaMenuItem("МАРГАРИТА", 450, "marg.png", b.GetResult(), false);
            director.MakePepperoni(b); itP = new PizzaMenuItem("ПЕППЕРОНИ", 550, "pep.png", b.GetResult(), false);
            director.MakeCountry(b); itK = new PizzaMenuItem("КАНТРИ", 500, "kant.png", b.GetResult(), false);
            director.MakeHawaiian(b); itH = new PizzaMenuItem("ГАВАЙСКАЯ", 520, "pineapple.png", b.GetResult(), false);
            director.MakeChicken(b); itCh = new PizzaMenuItem("КУРИНАЯ", 530, "chicken.png", b.GetResult(), false);

            flowMenu.Controls.AddRange(new Control[] { itC.CreatePanel(OnAdd), itM.CreatePanel(OnAdd), itP.CreatePanel(OnAdd), itK.CreatePanel(OnAdd), itH.CreatePanel(OnAdd), itCh.CreatePanel(OnAdd) });
            UpdateUI();
        }

        private void OnAdd(object sender, EventArgs e)
        {
            PizzaMenuItem sel = null; Panel p = (Panel)((Button)sender).Parent;
            if (p == itC.Panel) sel = itC;
            else if (p == itM.Panel) sel = itM;
            else if (p == itP.Panel) sel = itP;
            else if (p == itK.Panel) sel = itK; else if (p == itH.Panel) sel = itH; else if (p == itCh.Panel) sel = itCh;

            if (sel != null)
            {
                var pizza = sel.CreatePizzaWithToppings();
                order.AddPizza(pizza);
                var item = order.Items.Last();
                extraInfo[item] = sel.GetUserSelectedExtras();
                UpdateUI();
                foreach (var ctrl in sel.ToppingControls) ctrl.Value.Value = 0;
            }
        }

        private void UpdateUI()
        {
            flowOrder.Controls.Clear();
            foreach (var item in order.Items)
            {
                Panel card = new Panel { Size = new Size(365, 145), BackColor = Color.White, Margin = new Padding(0, 0, 0, 15) };
                card.Paint += (s, e) => ControlPaint.DrawBorder(e.Graphics, card.ClientRectangle, Color.FromArgb(230, 230, 230), ButtonBorderStyle.Solid);

                Label lblN = new Label { Text = item.Pizza.name, Font = new Font("Segoe UI", 11, FontStyle.Bold), Location = new Point(15, 10), AutoSize = true };
                Label lblD = new Label { Text = $"{item.Pizza.Dough.name} / {item.Pizza.Sauce.name}", Location = new Point(15, 32), ForeColor = Color.Gray, Font = new Font("Segoe UI", 8), AutoSize = true };

                string extras = (extraInfo.ContainsKey(item) && extraInfo[item].Count > 0)
                    ? "Доп: " + string.Join(", ", extraInfo[item])
                    : (item.Pizza.custom ? "Пустая пицца" : "Стандартный состав");

                Label lblT = new Label { Text = extras, Location = new Point(15, 50), Size = new Size(335, 45), Font = new Font("Segoe UI", 7), ForeColor = Color.DimGray };
                Label lblP = new Label { Text = $"{item.GetTotalPrice()} ₽", Location = new Point(15, 105), Font = new Font("Segoe UI", 12, FontStyle.Bold), ForeColor = Color.OrangeRed, AutoSize = true };

                // Компактный счетчик
                Panel qtyPnl = new Panel { Size = new Size(110, 36), Location = new Point(235, 100), BorderStyle = BorderStyle.FixedSingle };
                Button btnM = new Button { Text = "-", Size = new Size(32, 34), Location = new Point(0, 0), FlatStyle = FlatStyle.Flat };
                btnM.Click += (s, e) => { if (item.Quantity > 1) item.Quantity--; else order.RemoveItem(item); UpdateUI(); };
                Button btnP = new Button { Text = "+", Size = new Size(32, 34), Location = new Point(76, 0), FlatStyle = FlatStyle.Flat };
                btnP.Click += (s, e) => { item.Quantity++; UpdateUI(); };
                Label lblQ = new Label { Text = item.Quantity.ToString(), Size = new Size(44, 34), Location = new Point(32, 0), TextAlign = ContentAlignment.MiddleCenter, Font = new Font("Segoe UI", 10, FontStyle.Bold) };

                qtyPnl.Controls.AddRange(new Control[] { btnM, lblQ, btnP });
                card.Controls.AddRange(new Control[] { lblN, lblD, lblT, lblP, qtyPnl });
                flowOrder.Controls.Add(card);
            }
            lblSum.Text = $"{order.GetTotalSum()} ₽";
            lblCount.Text = $"К оплате ({order.GetTotalPizzasCount()} шт.):";
        }

        private void BtnPay_Click(object sender, EventArgs e)
        {
            if (order.Items.Count == 0) return;
            string report = $"ЗАКАЗ [{DateTime.Now:HH:mm}]\n" + string.Join("\n", order.Items.Select(i =>
                $"• {i.Pizza.name} x{i.Quantity} ({i.GetTotalPrice()}₽)\n  Основа: {i.Pizza.Dough.name}, {i.Pizza.Sauce.name}\n  Допы: {(extraInfo.ContainsKey(i) ? string.Join(", ", extraInfo[i]) : "-")}"));
            history.Insert(0, report + $"\nСУММА: {order.GetTotalSum()}₽\n" + new string('=', 40));
            order = new Order(); extraInfo.Clear(); UpdateUI();
            MessageBox.Show("Заказ оформлен!", "Pizza Shop");
        }
    }
}