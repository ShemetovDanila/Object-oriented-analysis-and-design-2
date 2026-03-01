using System.Collections.Generic;

namespace WITH_pattern
{
    public class PizzaBuilder : IPizzaBuilder
    {
        private Pizza _pizza;

        public IPizzaBuilder Reset()
        {
            // Инициализируем новый объект Pizza и пустой список топпингов
            _pizza = new Pizza { Toppings = new List<Topping>() };
            return this;
        }

        public IPizzaBuilder SetIsCustom(bool custom)
        {
            _pizza.custom = custom;
            return this;
        }

        public IPizzaBuilder SetName(string name)
        {
            _pizza.name = name;
            return this;
        }

        public IPizzaBuilder SetBasePrice(int price)
        {
            _pizza.price = price;
            return this;
        }

        public IPizzaBuilder SetDough(string dough)
        {
            _pizza.Dough = ToppingCatalog.GetDoughByName(dough);
            return this;
        }

        public IPizzaBuilder SetSauce(string sauce)
        {
            _pizza.Sauce = ToppingCatalog.GetSauceByName(sauce);
            return this;
        }

        public IPizzaBuilder AddTopping(string topping)
        {
            var t = ToppingCatalog.GetToppingByName(topping);
            if (t != null)
            {
                _pizza.Toppings.Add(t);
            }
            return this;
        }

        public Pizza GetResult() => _pizza;
    }
}