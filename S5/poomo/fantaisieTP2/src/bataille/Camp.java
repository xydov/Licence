package bataille;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import protagoniste.EtreVivant;
import protagoniste.Monstre;

public class Camp<T extends EtreVivant> implements Iterable<T>{
	
	private List<T> liste = new LinkedList<T>();
	
	public void ajouter(T etreVivant) {
		liste.add(etreVivant);
	}
	
	public void eliminer(T etreVivant) {
		liste.remove(etreVivant);
	}
	
	public String toString() {
		return liste.toString();
		}
	
	public Iterator<T> iterator() {
		return liste.iterator();
	}	
}