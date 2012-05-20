#include "Util.h"
#include "TrackerNode.h"
#include "math/common.h"
using namespace std;

TrackerNode :: TrackerNode()
{
    //m_wpFocus = NULL;
    m_FocusFlag = FOCUS_NONE;
    m_bNeedsUpdate = false;
}

TrackerNode :: TrackerNode(weak_ptr<Node> nfocus, TrackerNode::FocusFlag ff)
{
    //_follow(t, ts);
    focus(nfocus, ff);
}

void TrackerNode :: update()
{
    if(shared_ptr<Node> focus = m_wpFocus.lock())
    {
        if(m_FocusFlag == FOCUS_ATTACH)
        {
            Matrix::translation(m_Matrix, Matrix::translation(*focus->matrix()));
        }
        else if(m_FocusFlag == FOCUS_ORIENT)
        {
            glm::vec3 pos = Matrix::translation(*focus->matrix());
            m_Matrix = *focus->matrix();
            //m_Matrix.clear(Matrix::COPY, *focus->matrix());
            Matrix::translation(m_Matrix, pos);
        }
        else if(m_FocusFlag == FOCUS_COPY)
        {
            m_Matrix = *focus->matrix();
            //m_Matrix.clear(Matrix::COPY, *focus->matrix());
        }

        m_bNeedsUpdate = false;
    }
}


glm::mat4* TrackerNode :: matrix()
{
    if(m_bNeedsUpdate)
        update();
    return &m_Matrix;
}

void TrackerNode :: focus(weak_ptr<Node> nfocus, FocusFlag ff)
{
    //if(!nfocus)
    //{
    //    m_wpFocus = NULL;
    //    return;
    //}

    if(nfocus.lock() == m_wpFocus.lock())
        return;

    m_wpFocus = nfocus;
    m_FocusFlag = ff;
    m_bNeedsUpdate = true;
}
