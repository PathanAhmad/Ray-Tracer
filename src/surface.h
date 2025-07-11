class Surface {
public:
    virtual ~Surface() = default;
    virtual bool intersect( const Ray& ray, Hit& hit ) const = 0;
}; 