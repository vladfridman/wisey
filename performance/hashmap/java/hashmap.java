import java.util.HashMap;
import java.util.Map;
import java.util.Iterator;
import java.util.Set;

public class hashmap {

  private static int ARRAY_SIZE = 1024000;

  public static void main(String args[]) {
    HashMap<Element, Element> mMap = new HashMap<Element, Element>();
    Element[] mArray = new Element[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; i++) {
      mArray[i] = new Element(i);
    }
    for (int j = 0; j < 10; j++) {
      for (int i = 0; i < ARRAY_SIZE; i++) {
        mMap.put(mArray[i], mArray[i]);
      }
    }
    long sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
      sum += mMap.get(mArray[i]).getKey();
    }
    mMap.clear();
    System.out.println("sum = " + sum);
  }

  public static class Element {
    int mKey;

    Element(int key) {
      mKey = key;
    }

    public int getKey() {
      return mKey;
    }
  }
}
