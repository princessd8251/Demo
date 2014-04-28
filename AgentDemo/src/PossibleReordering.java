import java.lang.Thread;

public class PossibleReordering {
	static int x = 0, y = 0;
	static int a = 0, b = 0;
	
	public static void main(String[] args) throws InterruptedException {
		
		Thread one = new Thread(new Runnable() {
			public void run() {
				for(int i=0; i<1; i++) {
					a = 1;
					x = b;
				}
			}
		});
		
		Thread other = new Thread(new Runnable() {
			
			public void run() {
				for (int i =0; i<1; i++) {
					b = 1;
					y = a;
					a = 1;
					x = b;
					a = 1;
					x = b;
					a = 1;
					x = b;
					a = 1;
					x = b;
					a = 1;
					x = b;
					a = 1;
					x = b;
					a = 1;
					x = b;
				}
			}
		});
		
		one.start();
		other.start();
		one.join();
		other.join();
	}
}
