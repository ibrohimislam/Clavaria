package me.ibrohim.clavaria;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.FileNotFoundException;
import java.io.InputStream;
import java.lang.reflect.Array;



public class MainActivity extends AppCompatActivity {

    private static final int INTENT_SELECT_PHOTO = 100;

    private ImageView iv;
    private TextView tv;

    private Bitmap srcImage;
    private Bitmap dstImage;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tv = (TextView) findViewById(R.id.sample_text);
        iv = (ImageView) findViewById(R.id.imageView);

        iv.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent photoPickerIntent = new Intent(Intent.ACTION_PICK);
                photoPickerIntent.setType("image/*");
                startActivityForResult(photoPickerIntent, INTENT_SELECT_PHOTO);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent imageReturnedIntent) {
        super.onActivityResult(requestCode, resultCode, imageReturnedIntent);

        switch(requestCode) {
            case INTENT_SELECT_PHOTO:
                if (resultCode == RESULT_OK) {
                    Uri selectedImage = imageReturnedIntent.getData();
                    InputStream imageStream = null;

                    try {
                        imageStream = getContentResolver().openInputStream(selectedImage);
                    } catch (FileNotFoundException e) {}

                    srcImage = BitmapFactory.decodeStream(imageStream);
                    iv.setImageBitmap(srcImage);

                    new Thread(new Runnable() {
                        public void run() {
                            processImage(srcImage);
                        }
                    }).start();

                }
        }
    }

    private void processImage(Bitmap src){
        final int width = src.getWidth();
        final int height = src.getHeight();

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tv.setText("processing...");
            }
        });

        int[] image = new int[width * height];
        src.getPixels(image, 0, width, 0, 0, width, height);

        final long startTime = System.nanoTime();
        int[] newImage = grayscale(image, width, height);
        final long endTime = System.nanoTime();


        Bitmap.Config conf = Bitmap.Config.ARGB_8888;

        if (dstImage == null) {
            dstImage = Bitmap.createBitmap(width, height, conf);
        }

        dstImage.setPixels(newImage, 0, width, 0, 0, width, height);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tv.setText("completed, took " + (endTime - startTime) + " ns");
                iv.setImageBitmap(dstImage);
            }
        });

    }

    public native int[] grayscale(int[] image, int width, int height);

    static {
        System.loadLibrary("native-lib");
    }
}
