set RTCTREE_NAMESERVERS=127.0.0.1
rtcwd /127.0.0.1/.host_cxt & ^
rtdis OpenPicts0.rtc & ^
rtdis mapsManager0.rtc & ^
rtdis imageInpaint0.rtc & ^
rtdis convToLineMap0.rtc & ^
rtdis controlPanel0.rtc & ^
rtdis CVImgThreshold0.rtc & ^
rtdis CVFindContours0.rtc & ^
rtdis CVFindContours1.rtc & ^
rtdis CVInpaint0.rtc & ^
rtdis CVFeatureDetector0.rtc & ^
rtdis CVCanny0.rtc & ^
rtcon OpenPicts0.rtc:FileName controlPanel0.rtc:openFilePath & ^
rtcon OpenPicts0.rtc:backCameraImage mapsManager0.rtc:backCamImg & ^
rtcon mapsManager0.rtc:receiveCamImg OpenPicts0.rtc:sendCameraImage & ^
rtcon mapsManager0.rtc:receiveFlagData OpenPicts0.rtc:sendFlagData & ^
rtcon mapsManager0.rtc:tempFolderPath controlPanel0.rtc:tempFolderPath & ^
rtcon controlPanel0.rtc:srcImagePath mapsManager0.rtc:makeNormSrcImgPath & ^
rtcon mapsManager0.rtc:clickPoint controlPanel0.rtc:clickPoint & ^
rtcon mapsManager0.rtc:compNormalizedMap imageInpaint0.rtc:processedImage & ^
rtcon imageInpaint0.rtc:modifyImagePath controlPanel0.rtc:modifyImagePath & ^
rtcon CVImgThreshold0.rtc:srcImage mapsManager0.rtc:makeNormSrcImg & ^
rtcon CVFindContours0.rtc:srcImage mapsManager0.rtc:makeNormSrcImg & ^
rtcon CVFindContours0.rtc:thresholdImage CVImgThreshold0.rtc:thresholdImg & ^
rtcon CVInpaint0.rtc:inpaintSrcImage imageInpaint0.rtc:inpaintSrcImage & ^
rtcon CVInpaint0.rtc:maskImage imageInpaint0.rtc:inpaintMaskImage & ^
rtcon CVInpaint0.rtc:maskRectangle imageInpaint0.rtc:inpaintMaskArea & ^
rtcon imageInpaint0.rtc:tempFolderPath mapsManager0.rtc:tempFolderPathOut & ^
rtcon imageInpaint0.rtc:srcImage mapsManager0.rtc:makeNormSrcImg & ^
rtcon imageInpaint0.rtc:contoursRectangles CVFindContours0.rtc:contoursRectangles & ^
rtcon imageInpaint0.rtc:contoursConvex CVFindContours0.rtc:contoursConvex & ^
rtcon imageInpaint0.rtc:clickPoint controlPanel0.rtc:clickPoint & ^
rtcon imageInpaint0.rtc:draggedRect controlPanel0.rtc:dragRectangle & ^
rtcon imageInpaint0.rtc:inpaintImage CVInpaint0.rtc:inpaintImage & ^
rtcon mapsManager0.rtc:compLineMap convToLineMap0.rtc:compLineMap & ^
rtcon CVFeatureDetector0.rtc:srcImage mapsManager0.rtc:makeLineSrcImg & ^
rtcon CVCanny0.rtc:srcImage mapsManager0.rtc:makeLineSrcImg & ^
rtcon CVFindContours1.rtc:srcImage mapsManager0.rtc:makeLineSrcImg & ^
rtcon CVFindContours1.rtc:thresholdImage CVCanny0.rtc:cannyImage & ^
rtcon convToLineMap0.rtc:tempFolderPath mapsManager0.rtc:tempFolderPathOut & ^
rtcon convToLineMap0.rtc:srcImage mapsManager0.rtc:makeLineSrcImg & ^
rtcon convToLineMap0.rtc:contoursData CVFindContours1.rtc:contoursData & ^
rtcon convToLineMap0.rtc:featurePoints CVFeatureDetector0.rtc:featurePoints & ^
rtcon convToLineMap0.rtc:clickPoint controlPanel0.rtc:clickPoint & ^
rtcon convToLineMap0.rtc:draggedRect controlPanel0.rtc:dragRectangle & ^
rtcon controlPanel0.rtc:modifyImagePath convToLineMap0.rtc:modifyImagePath & ^
rtcon controlPanel0.rtc:completeImagePath mapsManager0.rtc:compLineImgPath & ^
rtcon controlPanel0.rtc:completeImagePath mapsManager0.rtc:compNormImgPath & ^
rtcon controlPanel0.rtc:srcImagePath mapsManager0.rtc:makeLineSrcImgPath & ^
rtcon mapsManager0.rtc:stepFlag controlPanel0.rtc:stepFlag & ^
rtcon convToLineMap0.rtc:stepFlag controlPanel0.rtc:stepFlag & ^
rtcon imageInpaint0.rtc:stepFlag controlPanel0.rtc:stepFlag