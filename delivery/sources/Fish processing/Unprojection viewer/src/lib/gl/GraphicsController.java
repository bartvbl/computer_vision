package lib.gl;

import static org.lwjgl.opengl.GL11.*;

import java.awt.Font;

import org.newdawn.slick.Color;
import org.newdawn.slick.TrueTypeFont;
import org.newdawn.slick.opengl.TextureImpl;

public class GraphicsController {
	private static final TrueTypeFont font = new TrueTypeFont(new Font("Arial", Font.PLAIN, 18), true);
	
	public static void drawString(double x, double y, String string) {
		glPushMatrix();
		glScaled(1, -1, 1);
		glTranslated(0, -18 + -2 * y, 0);
		glEnable(GL_TEXTURE_2D);
		TextureImpl.bindNone();
		Color fontColour = new Color(0, 0, 0);
		font.drawString((float)x, (float)y, string, fontColour);
		TextureImpl.unbind();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}
