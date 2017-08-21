#author:    zhxiaoq9
#date:      2017-05-12
#function:  Read all file names in current directory, and rename sequentially.
#           Note that it does not included subdirectory
import os

def rename(path, kind):
    #get all file names
    files = os.listdir(path)
    #process every filename
    index = 0
    for fullname in files:
        #begin to process if it is not a directory
        if(os.path.isdir(fullname) != True):
            #split post fix and rename
            postfix = os.path.splitext(fullname)[1]
            if(postfix == kind):
                index = index + 1
                newname = str(index).zfill(3) + postfix
                newfile = os.path.join(path,newname)
                oldfile = os.path.join(path,fullname)
                os.rename(oldfile,newfile)
                
if __name__ == '__main__':
    rename('../imgtrain/isplate/img_lib/Level_1', '.jpg')   
    rename('../imgtrain/isplate/img_lib/Level_2', '.jpg')
    rename('../imgtrain/isplate/img_lib/Level_3', '.jpg')
