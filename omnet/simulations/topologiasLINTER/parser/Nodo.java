import java.util.Vector;

public class Nodo{

	int id, regEntryLNID, regEntryState, regEntryLSID, regEntrySID, regEntryLevel;
	String regEntryID;
	boolean orphan;
	Vector children;
	
public Nodo(){
		this.id = 0;
		this.regEntryID = "";
		this.regEntryLNID = 0;
		this.regEntryState = 0;
		this.regEntryLSID = 0;
		this.regEntrySID = 0;
		this.regEntryLevel = 0;
		this.children = new Vector();
		this.orphan =true; 
}	

public Nodo(int ID, String regEntryID, int regEntryLNID, int regEntryState, int regEntryLSID, int regEntrySID, int regEntryLevel){
		this.id = ID;
		this.regEntryID = regEntryID;
		this.regEntryLNID = regEntryLNID;
		this.regEntryState = regEntryState;
		this.regEntryLSID = regEntryLSID;
		this.regEntrySID = regEntrySID;
		this.regEntryLevel = regEntryLevel;
		this.children = new Vector();
		this.orphan = true;
	}
	
	public void addChild(Nodo child){
		this.children.add(child);
	}
	
	public String printChildren(){
		String nuevalinea = System.getProperty("line.separator");
		String tree;
		String tabulado="\t";
		
		for(int i=0;i<this.regEntryLevel;i++){
			tabulado = tabulado + "\t";
		}
		
		tree = tabulado + "|" + nuevalinea;
		tree = tree + tabulado + "|" + nuevalinea;
		tree = tree + tabulado + "-> I am node with SID = " + this.regEntrySID + " and LNID = " + this.regEntryLNID + nuevalinea;
		if(children.size()!=0){
			for (int i = 0; i < children.size(); i++){
				tree = tree + ((Nodo)this.children.get(i)).printChildren();
			}
		}
		return tree;
	}
}
