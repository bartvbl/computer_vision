package lib.gl;

import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.util.glu.GLU.gluPerspective;

import java.nio.FloatBuffer;

import org.lwjgl.BufferUtils;
import org.lwjgl.LWJGLException;
import org.lwjgl.opengl.Display;
import org.lwjgl.opengl.DisplayMode;

public class RenderUtils {
	private static final float NEAR_POINT = 0.1f;
	private static final float FAR_POINT = 2500000f;
	
	private static final int DEFAULT_WINDOW_WIDTH = 1024;
	private static final int DEFAULT_WINDOW_HEIGHT = 768;
	
	public static void newFrame()
	{
		Display.update();
		Display.sync(60);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, Display.getWidth(), Display.getHeight()); 
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	}
	
	public static void set3DMode()
	{
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, ((float)Display.getWidth()/(float)Display.getHeight()), RenderUtils.NEAR_POINT, RenderUtils.FAR_POINT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	
	public static void set2DMode()
	{
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, Display.getWidth(), 0, Display.getHeight(), 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE); 
	}
	
	public static void initOpenGL(String windowTitle) throws LWJGLException
	{
		Display.setTitle(windowTitle);
		Display.setLocation(100, 100);
		Display.setDisplayMode(new DisplayMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT));
		Display.setResizable(true);
		Display.create();
		
		glViewport(0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f, (float)DEFAULT_WINDOW_WIDTH / (float)DEFAULT_WINDOW_HEIGHT, 0.1f, 10000000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		FloatBuffer params = (FloatBuffer) BufferUtils.createFloatBuffer(4).put(new float[]{0, 0, 0, 1}).rewind();
		glLight(GL_LIGHT0, GL_POSITION, params);
		glClearColor(1, 1, 1, 1);
		glClearDepth(1.0);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}

	public static void resetSettings() {
		glClearColor(94.0f/255.0f, 161.0f/255.0f, 255.0f/255.0f, 1f);
	}
}
