// Imports para el parser
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import java.io.File;
import java.util.Vector;
import java.lang.Math;

// Para escribir el fichero de salida
import java.io.BufferedWriter;
import java.io.FileWriter;

public class ProcessS11 {
 
  public static void main(String argv[]) {
 
    try {
	
	//System.out.println(argv[0]);
	//1st argv -> Input file (XML)
	File fXmlFile = new File(argv[0]);
	DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
	DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
	Document doc = dBuilder.parse(fXmlFile);
 	//opcional
	//http://stackoverflow.com/questions/13786607/normalization-in-dom-parsing-with-java-how-does-it-work
	doc.getDocumentElement().normalize(); 
	//System.out.println("Root element :" + doc.getDocumentElement().getNodeName());
 
	//2nd argv -> Output file 1 (topology file)
	String out_file = argv[1];
	BufferedWriter bw = new BufferedWriter(new FileWriter(out_file));
	
	//3rd argv -> Output file 2 (topology tree)
	String out_file2 = argv[2];
	BufferedWriter bw2 = new BufferedWriter(new FileWriter(out_file2));
	
	//4rd argv -> Output file 3 (attenuation matrix file)
	String out_file3 = argv[3];
	BufferedWriter bw3 = new BufferedWriter(new FileWriter(out_file3));
	
	//5th, 6th, 7th args -> attenuations
	//5th = the highest (with ourselves and with nodes which are two levels away from our level)
	//6th = the lowest (with nodes which are in our level)
	//7th = medium (with nodes which are one level away from our level)
	
	NodeList nList = doc.getElementsByTagName("macListRegDevices");
 
	//System.out.println("----------------------------");
 
	for (int temp = 0; temp < nList.getLength(); temp++) {
 
		Node nNode = nList.item(temp);
 
		//System.out.println("\nCurrent Element: " + nNode.getNodeName());
 
		if (nNode.getNodeType() == Node.ELEMENT_NODE) {
 
			Element eElement = (Element) nNode;
 
			/*System.out.println("MAC: " + eElement.getAttribute("regEntryID"));
			System.out.println("Nodo de nivel: " + eElement.getAttribute("regEntryLevel"));
			System.out.println("LNID: " + eElement.getAttribute("regEntryLNID"));
			System.out.println("LSID: " + eElement.getAttribute("regEntryLSID"));
			System.out.println("SID: " + eElement.getAttribute("regEntrySID"));*/
 
		}
	}	
	
	int niveles = 5;
	Vector nivel[] = new Vector[niveles];
	for (int i=0; i<niveles; i++){
		nivel[i] = new Vector();
	}
	
	for (int temp = 0; temp < nList.getLength(); temp++) {
 
		Node nNode = nList.item(temp);
 
		if (nNode.getNodeType() == Node.ELEMENT_NODE) {
 
			Element eElement = (Element) nNode;
			nivel[Integer.parseInt(eElement.getAttribute("regEntryLevel"))].add(new Nodo(0, eElement.getAttribute("regEntryID"), Integer.parseInt(eElement.getAttribute("regEntryLNID")), Integer.parseInt(eElement.getAttribute("regEntryState")), Integer.parseInt(eElement.getAttribute("regEntryLSID")), Integer.parseInt(eElement.getAttribute("regEntrySID")), Integer.parseInt(eElement.getAttribute("regEntryLevel"))));
		}
	}
	
	
	//Genera fichero de topologia
	int id_nodo=1;
	for (int i=0; i < niveles; i++){
		for (int ii = 0; ii < nivel[i].size(); ii++){
			Nodo nodo_temporal = (Nodo)nivel[i].get(ii);
			nodo_temporal.id = id_nodo++;
			nivel[i].setElementAt(nodo_temporal,ii);
			bw.write(nodo_temporal.regEntryLevel + "|" + nodo_temporal.id + "|" + nodo_temporal.regEntryLNID + "|" + nodo_temporal.regEntryLSID + "|" + nodo_temporal.regEntrySID +"|\n");	
		}
	}
	bw.close();
	
	//Genera fichero de arbol de topologia
		//Asigna nodos hijo a los nodos padre.
	for (int i=1; i < niveles; i++){
		for (int index_child = 0; index_child < nivel[i].size(); index_child++){
			Nodo node_child = (Nodo)nivel[i].get(index_child);
			
			Nodo node_father;
			for(int index_father = 0; index_father < nivel[i-1].size(); index_father++){
				node_father = (Nodo)nivel[i-1].get(index_father);
				if(node_father.regEntryLSID==node_child.regEntrySID){
					node_child.orphan=false;
					nivel[i].setElementAt(node_child,index_child);
					node_father.addChild(node_child);
					nivel[i-1].setElementAt(node_father,index_father);
				}
			}
		}
	}
		//Imprime el arbol de topologia para todos los nodos raiz.
	String nuevalinea = System.getProperty("line.separator");
	bw2.write("The network logical topology is :"+nuevalinea+"I am node with SID=0"+nuevalinea);
	for(int index_root = 0; index_root < nivel[0].size(); index_root++){
		Nodo node_root = (Nodo)nivel[0].get(index_root);
		bw2.write(node_root.printChildren());
	}

		//Busca e imprime nodos marcados como huérfanos
	for(int i=0;i < niveles;i++){
		for(int j=0;j < nivel[i].size();j++){
			if(((Nodo)nivel[i].get(j)).orphan){
				bw2.write("Nodo huerfano: " + ((Nodo)nivel[i].get(j)).id + "\n");
			}
		}
	}

	bw2.close();
	
	nivel[0].add(new Nodo());
	//Genera fichero de matriz de atenuaciones
	for (int level = 0; level < niveles ; level++){
		//System.out.println("Pasando por nivel "+level+"\n");
		for (int pos = 0; pos < nivel[level].size(); pos++){
			//System.out.println("Pasando por la posicion "+pos+" del nivel "+level+"\n");
			for (int i=0; i < niveles; i++){
				//System.out.println("Recorriendo nivel "+i+"\n");
				for (int j = 0; j < nivel[i].size(); j++){
					if(level==i){
						if (j==pos){
							//System.out.println("Soy yo mismo "+argv[4]+"\n");
							bw3.write(argv[4]+"|");
						}
						else{
							//System.out.println("Es de mi nivel "+argv[5]+"\n");
							bw3.write(argv[5]+"|");
						}
					}
					else if (Math.abs(level-i)==1){
							//System.out.println("Esta a un nivel de distancia "+argv[5]+"\n");
							bw3.write(argv[6]+"|");
					}
					else{
						//System.out.println("Esta a mas de un nivel de distancia "+argv[3]+"\n");
						bw3.write(argv[4]+"|");
					}
				}
			}
			bw3.write("\n");	
		}
	}
	bw3.close();
	
    } catch (Exception e) {
	e.printStackTrace();
    }
  }
 
}
