import csv
import math
import sys
import numpy as np
from numpy import array
from sklearn import tree
from random import shuffle
import operator
# import matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

from sklearn import tree
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC
from sklearn.svm import LinearSVC

# from sklearn.preprocessing import normalize

star_file = str(sys.argv[1])
TRAIN_NUM = 800
lines = []
data = []
target = []

def getSignName(num):
    return {
        1 : '摩羯座',
        2 : '水瓶座',
        3 : '雙魚座',
        4 : '牡羊座',
        5 : '金牛座',
        6 : '雙子座',
        7 : '巨蟹座',
        8 : '獅子座',
        9 : '處女座',
        10: '天秤座',
        11: '天蠍座',
        12: '射手座'
    }[num]

def getAttrName(num):
    return {
        1 : '耐性',
        2 : '脾氣暴躁',
        3 : '幼稚',
        4 : '頑固',
        5 : '心思細膩',
        6 : '保守',
        7 : '冷靜',
        8 : '樂觀',
        9 : '活潑',
        10: '公正',
        11: '優柔寡斷',
        12: '強勢',
        13: '浪漫',
        14: '過度理想化',
        15: '斤斤計較',
        16: '心機重',
        17: '完美主義',
        18: '愛計仇',
        19: '與眾不同',
        20: '愛面子',
        21: '有魅力',
        22: '正義感',
        23: '重視友情',
        24: '專情',
        25: '愛哭',
        26: '顧家',
        27: '體貼',
        28: '情緒化',
        29: '口才',
        30: '創意',
        31: '潔癖'
    }[num]

# 計算各星座的特質
def calAttr(signAttr, signData):
    for i in range(1, 13): # 12 個星座
        for a in range(1, 32): # 32 題
            signAttr[getSignName(i)][getAttrName(a)] /= len(signData[getSignName(i)])

# 列出每個星座前n高的特質
def highestAttr(signAttr, n):
    for i in range(1, 13): # 12 個星座
        sorted_a = sorted(signAttr[getSignName(i)].items(), key=operator.itemgetter(1))
        sorted_a.reverse()
        print('---' + getSignName(i) + '---\n')
        a = 0
        for attr in range(32):
            if (sorted_a[attr][0] == '專情') or (sorted_a[attr][0] == '重視友情'):
                continue
            a += 1
            print(sorted_a[attr])
            if a == 5:
                break
        print('\n\n')

# 每個特質最高與最低的n個星座
def maxAttrSign(signAttr, n):
    for a in range(1, 32):
        attr = {}
        for s in range(1, 13):
            attr[getSignName(s)] = signAttr[getSignName(s)][getAttrName(a)]

        sorted_attr = sorted(attr.items(), key=operator.itemgetter(1))
        sorted_attr.reverse()

        print('--- ' + getAttrName(a) + '---')
        print('\n前三名\n')
        for i in range(n):
            print(sorted_attr[i])
        print('\n後三名\n')
        for i in range(n):
            print(sorted_attr[11-i])
        print('\n\n')


starSignData = {}
starSignAttr = {}
otherSignData = {}
otherSignAttr = {}
train_count = {}
# attrData = {}

a = {}
d = {}
for i in range(1, 32):
    a[getAttrName(i)] = 0
    d[getAttrName(i)] = []

attrData = dict(d)
stddev = dict(a)

for i in range(1, 13):
    starSignData[getSignName(i)] = []
    starSignAttr[getSignName(i)] = dict(a)
    otherSignData[getSignName(i)] = []
    otherSignAttr[getSignName(i)] = dict(a)
    train_count[getSignName(i)] = 0

# main
tree_avg_sum = 0
knn_avg_sum = 0
nb_avg_sum = 0
svc_avg_sum = 0
rf_avg_sum = 0

# read star.csv
with open(star_file, newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
    i = 0
    for row in spamreader:
        if i == 0: # header
            row = row[0].split(',')
            lines.append(row)
            i += 1
        else:
            row = row[2].split(',')
            lines.append(row)

feature_names = lines[0]


# print result header
del feature_names[0] # remove time stamp
del feature_names[len(feature_names)-2] # remove commands
# print(feature_names)
print('\n\n===== star-sign data set =====\n')
print('資料總數 = %d 份\n' % len(lines))
total_num = len(lines)
other_total = 0

# lines
del lines[0]
i = 0
for line in lines:
    del line[0] # remove time
    del line[len(line)-2]

i = 0
for line in lines:
    if len(line) != 33:
        # print('line[%d] has incorrect element num' % i)
        total_num -= 1
        continue

    d = {}
    d['分析對象'] = line[0]
    d['星座'] = line[1]
    d['耐性'] = int(line[2])
    d['脾氣暴躁'] = int(line[3])
    d['幼稚'] = int(line[4])
    d['頑固'] = int(line[5])
    d['心思細膩'] = int(line[6])
    d['保守'] = int(line[7])
    d['冷靜'] = int(line[8])
    d['樂觀'] = int(line[9])
    d['活潑'] = int(line[10])
    d['公正'] = int(line[11])
    d['優柔寡斷'] = int(line[12])
    d['強勢'] = int(line[13])
    d['浪漫'] = int(line[14])
    d['過度理想化'] = int(line[15])
    d['斤斤計較'] = int(line[16])
    d['心機重'] = int(line[17])
    d['完美主義'] = int(line[18])
    d['愛計仇'] = int(line[19])
    d['與眾不同'] = int(line[20])
    d['愛面子'] = int(line[21])
    d['有魅力'] = int(line[22])
    d['正義感'] = int(line[23])
    d['重視友情'] = int(line[24])
    d['專情'] = int(line[25])
    d['愛哭'] = int(line[26])
    d['顧家'] = int(line[27])
    d['體貼'] = int(line[28])
    d['情緒化'] = int(line[29])
    d['口才'] = int(line[30])
    d['創意'] = int(line[31])
    d['潔癖'] = int(line[32])
    data.append(d)
    i += 1

# 將資料依星座、特質分類並加總特質
for row in data:
    for i in range(1, 32):
        starSignAttr[row['星座']][getAttrName(i)] += row[getAttrName(i)]
        attrData[getAttrName(i)].append(row[getAttrName(i)])
        if row['分析對象'] == '他人':
            otherSignAttr[row['星座']][getAttrName(i)] += row[getAttrName(i)]

    for i in range(1, 13):
        if(row['星座'] == getSignName(i)):
            starSignData[getSignName(i)].append(row)
            if row['分析對象'] == '他人':
                otherSignData[getSignName(i)].append(row)
                other_total += 1
# test
# test_num = total_num - TRAIN_NUM - 2
# print('\n測試資料 = %d 筆\n' % test_num)

# 印出各星座資料數量
for i in range(1, 13):
    print(getSignName(i) + ' = %d' % len(starSignData[getSignName(i)]))


# 計算各特質標準差

# print('\n===== 各特質標準差 =====\n')
for i in range(1, 32):
    sd = np.std(array(attrData[getAttrName(i)]))
    stddev[getAttrName(i)] = sd

sorted_stddev = sorted(stddev.items(), key=operator.itemgetter(1))
sorted_stddev.reverse()

# for i in range(0, 31):
#     print(sorted_stddev[i])




# 輸入執行次數
dt_list = []
rf_list = []
knn_list = []
svm_list = []
nb_list = []
t_list = []


print('\n===== testing =====\n')
time = int(input("execute time: "))

for x in range(time):
    print('\n\n--- test %d ---' % (x+1))
    t_list.append(x+1)

    # 挑 training set
    shuffle(data)
    train_data = []
    train_target = []
    test_data = []
    test_target = []
    test_num = 0
    for i in range(1, 13):
        train_count[getSignName(i)] = 0

    for i in range(total_num-1):
        r = []
        for a in range(1, 32):
            # if(getAttrName(a) != '重視友情'):
            r.append(data[i][getAttrName(a)])
        if train_count[data[i]['星座']] < TRAIN_NUM:
            train_data.append(r)
            train_target.append(data[i]['星座'])
        else:
            test_data.append(r)
            test_target.append(data[i]['星座'])
            test_num += 1

        train_count[data[i]['星座']] += 1

    # decision tree
    dTree = tree.DecisionTreeClassifier()
    dTree = dTree.fit(train_data, train_target)
    # print('\n--- dTree ---')
    correct = 0

    for i in range(test_num):
        predict = dTree.predict([test_data[i]])
        if predict == test_target[i]:
            correct += 1

    acc = correct / test_num
    dt_list.append(acc)
    tree_avg_sum += acc

    print('\ndTree accuracy = %f' % acc)

    # random forest
    rf = RandomForestClassifier(random_state=0, n_estimators = 500)
    rf.fit(train_data, train_target)
    # print('\n--- dTree ---')
    correct = 0

    for i in range(test_num):
        predict = rf.predict([test_data[i]])
        if predict == test_target[i]:
            correct += 1

    acc = correct / test_num
    rf_avg_sum += acc
    rf_list.append(acc)
    print('\nrandom forest accuracy = %f' % acc)


    # KNN
    correct = 0
    knn = KNeighborsClassifier(n_neighbors = 400, algorithm="kd_tree").fit(train_data, train_target)
    # print('\n\n--- KNN ---')

    for i in range(test_num):
        predict = knn.predict([test_data[i]])
        if predict == test_target[i]:
            correct += 1

    acc = correct / test_num
    knn_avg_sum += acc
    knn_list.append(acc)
    print('\nknn accuracy = %f' % acc)

    # naïve Bayes
    # print('\n\n--- naïve Bayes ---')
    correct = 0
    nb = GaussianNB()
    nb.fit(train_data, train_target)

    for i in range(test_num):
        predict = nb.predict([test_data[i]])
        if predict == test_target[i]:
            correct += 1

    acc = correct / test_num
    nb_avg_sum += acc
    nb_list.append(acc)
    print('\nnaïve Bayes accuracy = %f' % acc)

    # SVM
    svc = SVC(random_state=0, degree = 20)
    svc.fit(train_data, train_target)
    correct = 0

    for i in range(test_num):
        predict = svc.predict([test_data[i]])
        if predict == test_target[i]:
            correct += 1

    acc = correct / test_num
    svc_avg_sum += acc
    svm_list.append(acc)
    print('\nSVM accuracy = %f' % acc)



# avg test result
# plt.plot(k_list, acc_list, '-o')

plt.ylabel('accuracy')
# plt.xlabel('dTree')
plt.plot(t_list, dt_list, label='dTree' )
# plt.xlabel('random forest')
plt.plot(t_list, rf_list, label='random forest')
# plt.xlabel('knn')
plt.plot(t_list, knn_list, label='knn')
# plt.xlabel('naive bayes')
plt.plot(t_list, nb_list, label='naive bayes')
plt.plot(t_list, svm_list, label='svm')
plt.legend()
plt.savefig(sys.argv[2])
# plt.plot(k_list, rf_acc_list)
# plt.savefig('rf_100.png')
# plt.show()
print()
