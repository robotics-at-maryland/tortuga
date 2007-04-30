/* OpenCV include */
#include "cv.h"
#include "highgui.h"

/* FFMpeg library */
#include "avcodec.h"
#include "avformat.h"

#define        WIDTH            356
#define        HEIGHT            292
#define        BYTEPIC            (WIDTH * HEIGHT * 3) * 200

AVFrame *alloc_picture420P(int width, int height) {
  AVFrame *picture;
  uint8_t *picture_buf;
  int size;
  int pix_fmt = PIX_FMT_YUV420P;
  picture = avcodec_alloc_frame();
  if (!picture) return NULL;
  size = avpicture_get_size(pix_fmt, width, height);
  picture_buf = (uint8_t *) malloc(size);
  if (!picture_buf) {
    av_free(picture);
    return NULL;
  }
  avpicture_fill((AVPicture *)picture, picture_buf, pix_fmt, width, height);

//You must change the path to your video including media folder, i.e. 'media/test.wmv'.
  return picture;
}

AVFrame *alloc_pictureBGR24(int width, int height) {
  AVFrame *pFrameRGB = avcodec_alloc_frame();
  if(pFrameRGB==NULL) return NULL;
  int numBytes = avpicture_get_size(PIX_FMT_BGR24, width, height);
  uint8_t *buffer=new uint8_t[numBytes];
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_BGR24, width, 
height);
  return pFrameRGB;
}

int main() {
  CvCapture *capture;
  IplImage *src, *dst, *dst2;
  AVCodec *codec, *decodec;
  AVCodecContext *c, *d;

  capture = cvCaptureFromCAM(0);

  dst = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3 );
  dst2 = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3 );

  c = NULL;
  av_register_all();

  int out_size, size;

  codec = avcodec_find_encoder(CODEC_ID_MPEG4);
  if (!codec) {
    //MessageBox(NULL,"Codec not found","Errore",0);
    exit(1);
  }

  decodec = avcodec_find_decoder(CODEC_ID_MPEG4);
  if (!codec) {
    //MessageBox(NULL,"Decoder not found","Errore",0);
    exit(1);
  }

  AVFrame *picture, *picture2;
  AVFrame *pictureBGR, *pictureBGR2;

  c = avcodec_alloc_context();
  d = avcodec_alloc_context();

  c->bit_rate = 400000;
  c->pix_fmt = PIX_FMT_YUV420P;
  c->width = WIDTH;
  c->height = HEIGHT;
    AVRational frame_rate = {1,25};
    c->time_base = frame_rate;
  //c->frame_rate_base = 1;
  c->gop_size = 10;
  c->max_b_frames = 1;

  d->bit_rate = 400000;
  d->pix_fmt = PIX_FMT_YUV420P;
  d->width = WIDTH;
  d->height = HEIGHT;

  d->time_base = frame_rate;
//  d->frame_rate_base = 1;
  d->gop_size = 10;
  d->max_b_frames = 1;

  if (avcodec_open(c,codec) < 0) {
//    MessageBox(NULL,"Could not open codec","Errore",0);
    exit(1);
  }

  if (avcodec_open(d,decodec) < 0) {
//    MessageBox(NULL,"Could not open decoder","Errore",0);
    exit(1);
  }

  uint8_t *outbuf = new uint8_t[BYTEPIC];

  cvNamedWindow( "Video 1" , CV_WINDOW_AUTOSIZE );
  cvMoveWindow( "Video 1", 0, 0);

  int got_picture, len = 0;

  printf("Allocing pictures\n");
  pictureBGR = alloc_pictureBGR24(WIDTH, HEIGHT);
  pictureBGR2 = alloc_pictureBGR24(WIDTH, HEIGHT);
  picture = alloc_picture420P(WIDTH, HEIGHT);
  picture2 = alloc_picture420P(WIDTH, HEIGHT);

  while (1/*!stopthread*/) {
    //Sleep(milliseconds);
    src = cvQueryFrame( capture ); // 320 x 240
    cvResize(src, dst);
    cvConvertImage(dst, dst, CV_CVTIMG_FLIP);

    for (int i = 0; i < dst->imageSize; i++)
    {
      pictureBGR->data[0][i] = dst->imageData[i];
    }

    printf("Converting images\n");
    img_convert((AVPicture *)picture, PIX_FMT_YUV420P, 
(AVPicture*)pictureBGR, PIX_FMT_BGR24, dst->width, dst->height);
    printf("Encode\n");
    out_size = avcodec_encode_video(c, outbuf, sizeof(outbuf), picture);

    if (out_size > 0) {
      printf("Decode\n");
      len = avcodec_decode_video(d, picture2, &got_picture, outbuf, 
out_size);
    }
    else
    {
      got_picture = 0;
    }

    if (got_picture > 0)
    {
      printf("Convert\n");
      img_convert((AVPicture *)pictureBGR2, PIX_FMT_BGR24, 
                  (AVPicture*)picture2, PIX_FMT_YUV420P, dst2->width, dst2->height);
      for (int i = 0; i < dst2->imageSize; i++)
      {
        dst2->imageData[i] = pictureBGR2->data[0][i];
      }
      cvShowImage( "Video 1" , dst2 );
      cvWaitKey(1);
    }
  }
}
