/****************************************************************************
** Meta object code from reading C++ file 'backend_draw_thread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../backend_draw_thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'backend_draw_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Backend_Draw_Thread_t {
    QByteArrayData data[10];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Backend_Draw_Thread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Backend_Draw_Thread_t qt_meta_stringdata_Backend_Draw_Thread = {
    {
QT_MOC_LITERAL(0, 0, 19), // "Backend_Draw_Thread"
QT_MOC_LITERAL(1, 20, 20), // "threadSigDrawOutPoly"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 2), // "x0"
QT_MOC_LITERAL(4, 45, 2), // "y0"
QT_MOC_LITERAL(5, 48, 2), // "x1"
QT_MOC_LITERAL(6, 51, 2), // "y1"
QT_MOC_LITERAL(7, 54, 7), // "color__"
QT_MOC_LITERAL(8, 62, 10), // "wakeStatus"
QT_MOC_LITERAL(9, 73, 23) // "threadSigDrawOutOnePoly"

    },
    "Backend_Draw_Thread\0threadSigDrawOutPoly\0"
    "\0x0\0y0\0x1\0y1\0color__\0wakeStatus\0"
    "threadSigDrawOutOnePoly"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Backend_Draw_Thread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    6,   24,    2, 0x06 /* Public */,
       9,    6,   37,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::ULong, QMetaType::Int,    3,    4,    5,    6,    7,    8,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::ULong, QMetaType::Int,    3,    4,    5,    6,    7,    8,

       0        // eod
};

void Backend_Draw_Thread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Backend_Draw_Thread *_t = static_cast<Backend_Draw_Thread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->threadSigDrawOutPoly((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< ulong(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6]))); break;
        case 1: _t->threadSigDrawOutOnePoly((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< ulong(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Backend_Draw_Thread::*_t)(float , float , float , float , unsigned long , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Backend_Draw_Thread::threadSigDrawOutPoly)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Backend_Draw_Thread::*_t)(float , float , float , float , unsigned long , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Backend_Draw_Thread::threadSigDrawOutOnePoly)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject Backend_Draw_Thread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Backend_Draw_Thread.data,
      qt_meta_data_Backend_Draw_Thread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Backend_Draw_Thread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Backend_Draw_Thread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Backend_Draw_Thread.stringdata0))
        return static_cast<void*>(const_cast< Backend_Draw_Thread*>(this));
    return QThread::qt_metacast(_clname);
}

int Backend_Draw_Thread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Backend_Draw_Thread::threadSigDrawOutPoly(float _t1, float _t2, float _t3, float _t4, unsigned long _t5, int _t6)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Backend_Draw_Thread::threadSigDrawOutOnePoly(float _t1, float _t2, float _t3, float _t4, unsigned long _t5, int _t6)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_Backend_Wait_Thread_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Backend_Wait_Thread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Backend_Wait_Thread_t qt_meta_stringdata_Backend_Wait_Thread = {
    {
QT_MOC_LITERAL(0, 0, 19) // "Backend_Wait_Thread"

    },
    "Backend_Wait_Thread"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Backend_Wait_Thread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void Backend_Wait_Thread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Backend_Wait_Thread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Backend_Wait_Thread.data,
      qt_meta_data_Backend_Wait_Thread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Backend_Wait_Thread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Backend_Wait_Thread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Backend_Wait_Thread.stringdata0))
        return static_cast<void*>(const_cast< Backend_Wait_Thread*>(this));
    return QThread::qt_metacast(_clname);
}

int Backend_Wait_Thread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
