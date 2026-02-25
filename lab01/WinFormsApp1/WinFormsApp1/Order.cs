using System;
using System.Collections.Generic;

namespace WinFormsApp1
{
    public class Order
    {
        public List<Pizza> Pizzas { get; set; }
        public DateTime OrderTime { get; set; }

        public Order()
        {
            Pizzas = new List<Pizza>();
            OrderTime = DateTime.Now;
        }

        public void AddPizza(Pizza pizza)
        {
            Pizzas.Add(pizza);
        }

        public int GetTotalSum()
        {
            int sum = 0;
            foreach (Pizza pizza in Pizzas)
            {
                sum += pizza.GetPrice();
            }
            return sum;
        }

        public string GetOrderDescription()
        {
            int margaritaCount = 0;
            int pepperoniCount = 0;
            int countryCount = 0;

            foreach (Pizza pizza in Pizzas)
            {
                if (pizza.Name == "Маргарита") margaritaCount++;
                else if (pizza.Name == "Пепперони") pepperoniCount++;
                else if (pizza.Name == "Кантри") countryCount++;
            }

            string description = $"Заказ на {GetTotalSum()} руб.: ";
            bool first = true;

            if (margaritaCount > 0)
            {
                description += $"{margaritaCount}x Маргарита";
                first = false;
            }
            if (pepperoniCount > 0)
            {
                if (!first) description += ", ";
                description += $"{pepperoniCount}x Пепперони";
                first = false;
            }
            if (countryCount > 0)
            {
                if (!first) description += ", ";
                description += $"{countryCount}x Кантри";
            }

            description += $" ({OrderTime.ToShortTimeString()})";
            return description;
        }
    }
}