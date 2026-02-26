namespace WITH_pattern
{
    public class PizzaBuilder : IPizzaBuilder
    {
        private Pizza _pizza;

        public PizzaBuilder()
        {
            Reset();
        }

        public void Reset()
        {
            _pizza = new Pizza();
        }

        public IPizzaBuilder SetName(string name)
        {
            _pizza.Name = name;
            return this;
        }

        public IPizzaBuilder SetDough(Dough dough)
        {
            _pizza.Dough = dough;
            return this;
        }

        public IPizzaBuilder SetSauce(Sauce sauce)
        {
            _pizza.Sauce = sauce;
            return this;
        }

        public IPizzaBuilder SetBasePrice(int price)
        {
            _pizza.BasePrice = price;
            return this;
        }

        public IPizzaBuilder SetIsCustomPizza(bool isCustom)
        {
            _pizza.IsCustomPizza = isCustom;
            return this;
        }

        public IPizzaBuilder AddTopMozzarella(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Моцарелла");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopCheddar(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Чеддер");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopParmesan(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Пармезан");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopDorBlue(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Дор Блю");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopPepperoni(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Пепперони");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopHam(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Ветчина");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopChicken(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Курица");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopBacon(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Бекон");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopMushrooms(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Грибы");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopTomatoes(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Помидоры");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopOlives(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Оливки");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopJalapeno(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Перец халапено");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopBellPepper(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Болгарский перец");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopOnion(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Лук");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopPineapple(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Ананас");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopBasil(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Базилик");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public IPizzaBuilder AddTopArugula(int count)
        {
            if (count > 0)
            {
                Topping topping = ToppingCatalog.GetToppingByName("Руккола");
                _pizza.AddTopping(topping, count, isBase: false);
            }
            return this;
        }

        public Pizza GetResult()
        {
            Pizza result = _pizza;
            Reset();
            return result;
        }
    }
}