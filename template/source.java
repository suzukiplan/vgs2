package com.{Company}.{Project}
import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.Display;
import android.view.WindowManager;
import android.view.Window;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.PixelFormat;
import android.util.Log;
import java.io.InputStream;
import java.util.List;
//import com.google.android.gms.ads.AdRequest;
//import com.google.android.gms.ads.InterstitialAd;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;

public class {Project} extends Activity
{
	public static final String LOG_TAG="{Project}";
	public static int request;
	private AdView mAdView;
	private FrameLayout mLayout;
	private final int WC = FrameLayout.LayoutParams.WRAP_CONTENT;
	private boolean mAdViewVisible=false;

	public Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			switch(request) {
				case 1: // show ads
					if(null!=mAdView && !mAdViewVisible) {
						mAdView.setVisibility(View.VISIBLE);
						mAdViewVisible=true;
					}
					break;
				case 2: // delete ads
					if(null!=mAdView && mAdViewVisible) {
						mAdView.setVisibility(View.INVISIBLE);
						mAdViewVisible=false;
					}
					break;
				default:
					term();
					finish();
			}
			request=0;
		}
	};

	@Override
	public void onCreate(Bundle savedInstanceState) {
		try {
			super.onCreate(savedInstanceState);
			getWindow().setType(WindowManager.LayoutParams.TYPE_APPLICATION_MEDIA);
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
			getWindow().setFormat(PixelFormat.RGB_565);
			requestWindowFeature(Window.FEATURE_NO_TITLE);
			byte[] buf;
			Resources res=getResources();
			InputStream is=res.openRawResource(R.raw.romdata);
			buf=new byte[(int)is.available()];
			is.read(buf);
			setRomData(buf);
			setPause(0);
			WindowManager wm=(WindowManager)getSystemService(Context.WINDOW_SERVICE);
			Display disp=wm.getDefaultDisplay();
			if(0!=init((int)disp.getWidth(),(int)disp.getHeight())) {
				Log.e(LOG_TAG, "JNI FUNCTION ERROR: init()");
				System.exit(-1);
			} else {
				mLayout=new FrameLayout(this);

				float width=disp.getWidth();
				float height=width/320*50;
				mLayout.addView(new VgeSurfaceView(this,(int)height+2));
				getWindow().setContentView(mLayout);
				mLayout.addView(new PanelView(this,0,0,(int)width,(int)height,R.drawable.panel1));
				getWindow().setContentView(mLayout);

				int x=0;
				for(x=5;;x+=5) {
					if(!(320*x/10<=disp.getWidth() && 400*x/10<=disp.getHeight())) {
						break;
					}
				}
				x-=5;
				int gHeight=400*x/10;
				mLayout.addView(new PanelView(this,0,(int)height+2+gHeight,(int)width,(int)disp.getHeight(),R.drawable.panel2));
				getWindow().setContentView(mLayout);

				mAdView = new AdView(this);
				mAdView.setAdUnitId("ca-app-pub-adcode");
				mAdView.setAdSize(AdSize.BANNER);
				final FrameLayout.LayoutParams adParams = new FrameLayout.LayoutParams(WC,WC);
				adParams.gravity = (Gravity.TOP|Gravity.CENTER);
				mLayout.addView(mAdView,adParams);
				mAdView.loadAd(new AdRequest.Builder().build());
				mAdViewVisible=true;
				getWindow().setContentView(mLayout);
			}
		} catch(Exception e) {
			Log.e(LOG_TAG, "AN EXCEPTION DETECTED.", e);
			System.exit(-1);
		}
	}

	@Override
	protected void onPause() {
		setPause(1);
		super.onPause();
	}

	@Override
	protected void onStop() {
		super.onStop();
	}

	@Override
	protected void onResume() {
		super.onResume();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		System.exit(0);
	}

	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		if (event.getAction()==KeyEvent.ACTION_DOWN) {
			switch (event.getKeyCode()) {
				case KeyEvent.KEYCODE_BACK:
					setPause(1);
					return true;
			}
		}
		return super.dispatchKeyEvent(event);
	}
	public static native int setRomData(byte[] data);
	public static native int init(int sx,int sy);
	public static native int setVram(Bitmap bmp);
	public static native void setInput(int isOn,float x,float y);
	public static native void setPause(int p);
	public static native void term();
	public static native int getreq();
	static {
		System.loadLibrary("vgs2");
	}
}

class VgeSurfaceView extends SurfaceView implements SurfaceHolder.Callback,Runnable {
	public Thread thread;
	public boolean isAttached;
	private final int XSIZE=320;
	private final int YSIZE=400;
	private Bitmap vram;
	private Rect vramRect;
	private Rect screenRect;
	private Paint screenPaint;
	private int yPos;
	private int xPos;
	private int zoom;
	{Project} act;
	public static int viewHeight;

	public VgeSurfaceView(Context context,int y) {
		super(context);
		yPos=y;
		act=({Project})context;
		vram=Bitmap.createBitmap(XSIZE,YSIZE,Bitmap.Config.RGB_565);
		vram.setDensity(Bitmap.DENSITY_NONE);
		vramRect=new Rect(0,0,XSIZE,YSIZE);
		getHolder().addCallback(this);
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		for(zoom=5;;zoom+=5) {
			if(!(XSIZE*zoom/10<=width && YSIZE*zoom/10<=height)) {
				break;
			}
		}
		zoom-=5;
		int gWidth=XSIZE*zoom/10;
		int gHeight=YSIZE*zoom/10;
		xPos=(width-gWidth)/2;
		screenRect=new Rect(xPos,yPos,xPos+gWidth,yPos+gHeight);
		screenPaint=new Paint();
		screenPaint.setAntiAlias(false);
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		isAttached=true;
		thread = new Thread(this);
		thread.start();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		isAttached = false;
		while (thread.isAlive());
	}

	@Override
	public void run() {
		SurfaceHolder holder=getHolder();
		Canvas canvas;
		int rc=0;
		int rq;
		while (isAttached) {
			canvas=null;
			try {
				rc={Project}.setVram(vram);
				canvas=holder.lockCanvas();
				if(null!=canvas) {
					canvas.drawBitmap(vram,vramRect,screenRect,screenPaint);
				}
			} finally {
				if(null!=canvas) holder.unlockCanvasAndPost(canvas);
				if(rc!=0) break;
				rq={Project}.getreq();
				if(rq!=0) {
					{Project}.request=rq;
					act.handler.sendEmptyMessage(0);
				}
			}
		}
		if(0!=rc) {
			act.handler.sendEmptyMessage(0);
		}
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		float x=(event.getX()-xPos)/(zoom/10.0f)/2;
		float y=(event.getY()-yPos)/(zoom/10.0f)/2;
		if(1<event.getPointerCount()) {
			{Project}.setInput(0,x,y);
			if(3<=event.getPointerCount()) {
				{Project}.setPause(1);
			}
		}
		switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				{Project}.setInput(1,x,y);
				break;
			case MotionEvent.ACTION_UP:
				{Project}.setInput(0,x,y);
				break;
			case MotionEvent.ACTION_MOVE:
				{Project}.setInput(1,x,y);
				break;
			case MotionEvent.ACTION_CANCEL:
				{Project}.setInput(0,x,y);
				break;
		}
		return true;
	}
}

class PanelView extends View {
	public static final String LOG_TAG="PanelView";
	private int m_x;
	private int m_y;
	private int m_xs;
	private int m_ys;
	private int m_id;
	public PanelView(Context context,int x,int y,int xs,int ys,int id) {
		super(context);
		m_x=x;
		m_y=y;
		m_xs=xs;
		m_ys=ys;
		m_id=id;
	}
	@Override
	protected void onDraw(Canvas c) {
		int count = c.getSaveCount();
		c.save();
		Paint p = new Paint();
		Bitmap img;
		Resources res = this.getContext().getResources();
		img = BitmapFactory.decodeResource(res,m_id);
		BitmapDrawable drawable = new BitmapDrawable(img);
		drawable.setBounds(m_x,m_y,m_xs,m_ys);
		drawable.draw(c);
		c.restoreToCount(count);
	}
}
