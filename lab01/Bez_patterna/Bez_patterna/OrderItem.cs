using Bez_patterna;
using System;
using System.Windows.Forms;

namespace Bez_patterna
{
    public class OrderItem
    {
        public Pizza Pizza { get; set; }
        public int Quantity { get; set; }
        public Panel Panel { get; set; }
        public Label lblQuantity { get; set; }

        public OrderItem(Pizza pizza, int quantity)
        {
            Pizza = pizza;
            Quantity = quantity;
            Panel = new Panel();
            lblQuantity = new Label();
        }

        public int GetTotalPrice()
        {
            return Pizza.GetTotalPrice() * Quantity;
        }

        public void UpdateQuantityDisplay()
        {
            if (lblQuantity != null)
            {
                lblQuantity.Text = $"{Quantity}";
            }
        }
    }
}