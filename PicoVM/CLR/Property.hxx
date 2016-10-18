#ifndef PROPERTY_HXX
#define PROPERTY_HXX

template<typename T>
class AbstractProperty {
    public:
    virtual ~AbstractProperty() {}
    AbstractProperty() {}
    AbstractProperty(const T& v) : value(v) { }
    
    // Setter
    virtual T& operator=(const T& value) = 0;
    
    // Getters
    operator T& () { return value; }
    operator T const& () const { return value; }

    // Forced casting to original type
    T& operator ()() { return value; }
    const T& operator ()() const { return value; }

    protected:
    T value; 
};

template<typename T>
class Public : public AbstractProperty<T> {
    Public() : AbstractProperty<T>() {}
    Public(const T& v) : AbstractProperty<T>(v) { }
    T& operator=(const T& value) { return this->value = value; }
};


template<typename T, class X>
class ProtectedX : public AbstractProperty<T> {
    friend X;
    ProtectedX() : AbstractProperty<T>() {}
    ProtectedX(const T& v) : AbstractProperty<T>(v) { }
    protected:
    T& operator=(const T& value) { return this->value = value; }
};

template<typename T, class X, class Y>
class ProtectedXY : public AbstractProperty<T> {
    friend X;
    friend Y;
    ProtectedXY() : AbstractProperty<T>() {}
    ProtectedXY(const T& v) : AbstractProperty<T>(v) { }
    protected:
    T& operator=(const T& value) { return this->value = value; }
};

template<typename T, class X, class Y, class Z>
class ProtectedXYZ : public AbstractProperty<T> {
    friend X;
    friend Y;
    friend Z;
    ProtectedXYZ() : AbstractProperty<T>() {}
    ProtectedXYZ(const T& v) : AbstractProperty<T>(v) { }
    protected:
    T& operator=(const T& value) { return this->value = value; }
};

#endif
