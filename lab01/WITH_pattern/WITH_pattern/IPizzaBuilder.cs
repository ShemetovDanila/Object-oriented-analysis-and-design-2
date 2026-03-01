namespace WITH_pattern
{
    public interface IPizzaBuilder
    {
        IPizzaBuilder Reset();
        IPizzaBuilder SetName(string name);
        IPizzaBuilder SetBasePrice(int price);
        IPizzaBuilder SetIsCustom(bool custom);
        IPizzaBuilder SetDough(string dough);
        IPizzaBuilder SetSauce(string sauce);
        IPizzaBuilder AddTopping(string topping);
        Pizza GetResult();
    }
}