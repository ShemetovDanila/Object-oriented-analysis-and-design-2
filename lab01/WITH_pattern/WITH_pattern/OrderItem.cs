using System.Windows.Forms;

namespace WITH_pattern
{
    public class OrderItem
    {
        public Pizza Pizza { get; set; }
        public int Quantity { get; set; } = 1;
        public Label lblQuantity { get; set; }
        public Panel Panel { get; set; }

        public OrderItem(Pizza pizza)
        {
            Pizza = pizza;
        }

        public int GetTotalPrice()
        {
            return Pizza.CalculateTotal() * Quantity;
        }

        public void UpdateQuantityDisplay()
        {
            if (lblQuantity != null)
            {
                lblQuantity.Text = Quantity.ToString();
            }
        }
    }
}