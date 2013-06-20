#ifndef CREATECLOUD_H
#define CREATECLOUD_H

#include <core/GraphNode.h>


class CreateCloud : public core::GraphNode
{
    Q_OBJECT
public:
    typedef std::shared_ptr<CreateCloud> Ptr;

    CreateCloud();
    virtual void                  update( core::GraphNodeSocket *output )override;
};

#endif // CREATECLOUD_H

