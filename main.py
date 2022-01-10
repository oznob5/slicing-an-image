import os
import shutil
import gdal
import time


def make_direction(name):
    """Создание папки куда будут сохранятся обрезанные изображения
    Если такая папка уже существует, то содержимое удаляется"""
    if os.path.isdir(name):
        shutil.rmtree(name)
    os.mkdir(name)


def slice_image(work_dir):
    dataset = gdal.Open('d1.jpg')
    x_size = dataset.RasterXSize
    y_size = dataset.RasterYSize
    padding = 1000
    for i in range(0, x_size, padding):
        for j in range(0, y_size, padding):
            gdal.Translate('{}/img_{}_{}.jpg'.format(work_dir, j // padding, i // padding), dataset,
                           srcWin=[i, j, padding, padding])

def main():
    direction = 'chopped_image'
    make_direction(direction)  # создание папки, в ней будут лежать порезанные изображения
    slice_image(direction)


if __name__ == "__main__":
    start_time = time.time()
    main()
    print("%s seconds" % (time.time() - start_time))
