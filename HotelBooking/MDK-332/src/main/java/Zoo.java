import java.util.ArrayList;
import java.util.List;

public class Zoo {
    private List<Animal> animals = new ArrayList<>();

    public void addAnimal(Animal animal) {
        animals.add(animal);
    }

    public boolean removeAnimal(String name) {
        return animals.removeIf(a -> a.getName().equalsIgnoreCase(name));
    }

    public void printAll() {
        if (animals.isEmpty()) {
            System.out.println("Зоопарк пуст.");
            return;
        }
        for (Animal a : animals) System.out.println(a);
    }

    public double getDailyCostAll() {
        double sum = 0;
        for (Animal a : animals) sum += a.getDailyFoodCost();
        return sum;
    }

    public double getTotalCostAll() {
        double sum = 0;
        for (Animal a : animals) sum += a.getTotalCost();
        return sum;
    }
}


