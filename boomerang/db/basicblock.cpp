/*
 * Copyright (C) 1997-2001, The University of Queensland
 * Copyright (C) 2000-2001, Sun Microsystems, Inc
 * Copyright (C) 2002, Trent Waddington
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       basicblock.cc
 * OVERVIEW:   Implementation of the BasicBlock class.
 *============================================================================*/

/*
 * $Revision: 1.13 $
 * Dec 97 - created by Mike
 * 18 Apr 02 - Mike: Changes for boomerang
 * 04 Dec 02 - Mike: Added isJmpZ
 * 09 Jan 02 - Mike: Untabbed, reformatted
*/


/*==============================================================================
 * Dependencies.
 *============================================================================*/

#include <assert.h>
#if defined(_MSC_VER) && _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif 

#include "types.h"
#include "dataflow.h"
#include "exp.h"
#include "cfg.h"
#include "register.h"
#include "rtl.h"
#include "hllcode.h"
#include "proc.h"
#include "prog.h"
#include "util.h"

/**********************************
 * BasicBlock methods
 **********************************/

/*==============================================================================
 * FUNCTION:        BasicBlock::BasicBlock
 * OVERVIEW:        Constructor.
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
BasicBlock::BasicBlock()
    :
        m_DFTfirst(0),
        m_structType(NONE), m_loopCondType(NONE),
        m_loopHead(NULL), m_caseHead(NULL),
        m_condFollow(NULL), m_loopFollow(NULL),
        m_latchNode(NULL),
        m_nodeType(INVALID),
        m_pRtls(NULL),
        m_iLabelNum(0),
        m_labelneeded(false),
        m_bIncomplete(true),
        m_bJumpReqd(false),
        m_iNumInEdges(0),
        m_iNumOutEdges(0),
        m_iTraversed(false),
    m_returnVal(NULL),
// From Doug's code
ord(-1), revOrd(-1), inEdgesVisited(0), numForwardInEdges(-1), 
traversed(UNTRAVERSED), hllLabel(false), labelStr(0), 
indentLevel(0), immPDom(NULL), loopHead(NULL), caseHead(NULL), 
condFollow(NULL), loopFollow(NULL), latchNode(NULL), sType(Seq), 
usType(Structured) 
{
}

/*==============================================================================
 * FUNCTION:        BasicBlock::~BasicBlock
 * OVERVIEW:        Destructor.
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
BasicBlock::~BasicBlock() {
    if (m_pRtls)
    {
        // Delete the RTLs
        for (std::list<RTL*>::iterator it = m_pRtls->begin(); it != m_pRtls->end(); it++)
        {
            if (*it)
            {
                delete *it;
            }
        }

        // and delete the list
        delete m_pRtls;
        m_pRtls = NULL;
    }
    if (m_returnVal) delete m_returnVal;
}


/*==============================================================================
 * FUNCTION:        BasicBlock::BasicBlock
 * OVERVIEW:        Copy constructor.    
 * PARAMETERS:      bb - the BB to copy from
 * RETURNS:         <nothing>
 *============================================================================*/
BasicBlock::BasicBlock(const BasicBlock& bb)
    :   m_DFTfirst(0),
        m_structType(bb.m_structType), m_loopCondType(bb.m_loopCondType),
        m_loopHead(bb.m_loopHead), m_caseHead(bb.m_caseHead),
        m_condFollow(bb.m_condFollow), m_loopFollow(bb.m_loopFollow),
        m_latchNode(bb.m_latchNode),
        m_nodeType(bb.m_nodeType),
        m_pRtls(NULL),
        m_iLabelNum(bb.m_iLabelNum),
        m_labelneeded(false),
        m_bIncomplete(bb.m_bIncomplete),
        m_bJumpReqd(bb.m_bJumpReqd),
        m_InEdges(bb.m_InEdges),
        m_OutEdges(bb.m_OutEdges),
        m_iNumInEdges(bb.m_iNumInEdges),
        m_iNumOutEdges(bb.m_iNumOutEdges),
        m_iTraversed(false),
        m_returnVal(bb.m_returnVal),
// From Doug's code
ord(bb.ord), revOrd(bb.revOrd), inEdgesVisited(bb.inEdgesVisited), 
numForwardInEdges(bb.numForwardInEdges), traversed(bb.traversed), 
hllLabel(bb.hllLabel), labelStr(bb.labelStr), indentLevel(bb.indentLevel), 
immPDom(bb.immPDom), loopHead(bb.loopHead), caseHead(bb.caseHead), 
condFollow(bb.condFollow), loopFollow(bb.loopFollow), 
latchNode(bb.latchNode), sType(bb.sType), usType(bb.usType) 
{
    setRTLs(bb.m_pRtls);
}

/*==============================================================================
 * FUNCTION:        BasicBlock::BasicBlock
 * OVERVIEW:        Private constructor.
 * PARAMETERS:      pRtls -
 *                  bbType -
 *                  iNumOutEdges -
 * RETURNS:         <nothing>
 *============================================================================*/
BasicBlock::BasicBlock(std::list<RTL*>* pRtls, BBTYPE bbType, int iNumOutEdges)
    :   m_DFTfirst(0),
        m_structType(NONE), m_loopCondType(NONE),
        m_loopHead(NULL), m_caseHead(NULL),
        m_condFollow(NULL), m_loopFollow(NULL),
        m_latchNode(NULL),  
        m_nodeType(bbType),
        m_pRtls(NULL),
        m_iLabelNum(0),
        m_labelneeded(false),
        m_bIncomplete(false),
        m_bJumpReqd(false),
        m_iNumInEdges(0),
        m_iNumOutEdges(iNumOutEdges),
        m_iTraversed(false),
    m_returnVal(NULL),
// From Doug's code
ord(-1), revOrd(-1), inEdgesVisited(0), numForwardInEdges(-1), 
traversed(UNTRAVERSED), hllLabel(false), labelStr(0), 
indentLevel(0), immPDom(NULL), loopHead(NULL), caseHead(NULL), 
condFollow(NULL), loopFollow(NULL), latchNode(NULL), sType(Seq), 
usType(Structured) 
{
    m_OutEdges.reserve(iNumOutEdges);               // Reserve the space;
                                                    // values added with
                                                    // AddOutEdge()

    // Set the RTLs
    setRTLs(pRtls);

}

#if 0
/*==============================================================================
 * FUNCTION:        BasicBlock::storeUseDefineStruct
 * OVERVIEW:        Stores the type info of every register in this BB. This is 
 *          done by inserting the used/defined places for each register
 * PARAMETERS:      BBBlock 
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::storeUseDefineStruct(BBBlock& inBlock) 
{
    if (m_pRtls)                    // Can be zero if e.g. INVALID
    {

//      RTL_CIT rit;
        std::list<RTL*>_CIT rit;
        for (rit = m_pRtls->begin(); rit != m_pRtls->end(); rit++) {
        (*rit)->storeUseDefineStruct(inBlock);
        }
    }
}

/*==============================================================================
 * FUNCTION:        BasicBlock::propagateType
 * OVERVIEW:        Recursively propagates type information from one 
 *                  basic block to the next. Depth first traversal
 * PARAMETERS:      BasicBlock * prevBlock, used to find the parent BB 
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::propagateType(BasicBlock * prevBlock, bool endCase = false){

    // Remember to clear it before and after type propagation
    m_iTraversed = true;
    
    assert(usedDefineStruct != NULL);
    // First do any alias propagations in this BasicBlock
    usedDefineStruct->propagateRegWithinBB();

    tempPrevBB = prevBlock;
/*
    for (std::vector<PBB>::iterator it = m_OutEdges.begin(); it != m_OutEdges.end();
         it++) {
        PBB child = *it;

    if (child->m_iTraversed == false) {
        child->propagateType(this);
    }   
    }        
*/  

    if (!endCase) { 
    for (std::vector<PBB>::iterator it = m_OutEdges.begin(); it != m_OutEdges.end();
             it++) {

        PBB child = *it;    
        if (child->m_iTraversed == false) {
        child->propagateType(this);
        }
        else {
        child->propagateType(this, true);
        }
    }
    }    

    // We reached a back branch, propagate ONCE
    // and then leave
    // Ignore for now, implement later

    std::map<Byte, UDChain*>::iterator noDefMapIt;
    std::map<Byte, UDChain*>::iterator prevLastDefMapIt;

    std::map<Byte, UDChain*> noDefMap = usedDefineStruct->returnNoDefRegChains();

    // Iterate through the map of no def chains     
    for (noDefMapIt = noDefMap.begin(); noDefMapIt != noDefMap.end(); ++noDefMapIt){

    // Reset the forwardLooking BB to the immediate previous BB 
    BasicBlock * forwardLookingBB = tempPrevBB;

        int timeOut = 0;
    while (forwardLookingBB) {
        // Get a map of all the defined registers in the prev BB 
            std::map<Byte, UDChain*> prevLastDefMap = 
             forwardLookingBB->usedDefineStruct->returnLastDefRegChains();

        // See if it contains one of the reg that we don't have a def for
        prevLastDefMapIt = prevLastDefMap.find(noDefMapIt->first);

        if (prevLastDefMapIt != prevLastDefMap.end()){
        // Found a match! Propagate type. 

        if (prevLastDefMapIt->second->chainType > noDefMapIt->second->chainType){
            noDefMapIt->second->chainType = prevLastDefMapIt->second->chainType;
        }
        else{
            prevLastDefMapIt->second->chainType = noDefMapIt->second->chainType ;
        }
        break;
        }
        else {
        // 50 is currently an arbitary number   
        // If we look 50 basic blocks up and still don't find it 
        // where the register is defined, give up
        if (timeOut++ > 50)
           break;
                
        forwardLookingBB = forwardLookingBB->tempPrevBB;                

        // Complete loop, will go around until timeout. 
        // Break now to save time
        if (forwardLookingBB == this->tempPrevBB)
            break;          

        }
     }
    }  
}
#endif

/*==============================================================================
 * FUNCTION:        BasicBlock::getLabel
 * OVERVIEW:        Returns nonzero if this BB has a label, in the sense that a
 *                  label is required in the translated source code
 * PARAMETERS:      <none>
 * RETURNS:         An integer unique to this BB, or zero
 *============================================================================*/
int BasicBlock::getLabel() {
    return m_iLabelNum;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::isTraversed
 * OVERVIEW:        Returns nonzero if this BB has been traversed
 * PARAMETERS:      <none>
 * RETURNS:         True if traversed
 *============================================================================*/
bool BasicBlock::isTraversed() {
    return m_iTraversed;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::setTraversed
 * OVERVIEW:        Sets the traversed flag
 * PARAMETERS:      bTraversed: true to set this BB to traversed
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::setTraversed(bool bTraversed) {
    m_iTraversed = bTraversed;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::setRTLs
 * OVERVIEW:        Sets the RTLs for a basic block. This is the only place that
 *                  the RTLs for a block must be set as we need to add the back
 *                  link for a call instruction to its enclosing BB.
 * PARAMETERS:      rtls - a list of RTLs
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::setRTLs(std::list<RTL*>* rtls) {
    // should we delete old ones here?  breaks some things - trent
    m_pRtls = rtls;

    // Set the link between the last instruction (a call) and this BB
    // if this is a call BB
    HLCall* call = (HLCall*)*(m_pRtls->rbegin());
    if (call->getKind() == CALL_RTL)
        call->setBB(this);
}

void BasicBlock::setReturnVal(Exp *e) {
    if (m_returnVal) delete m_returnVal; 
    m_returnVal = e; 
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getType
 * OVERVIEW:        Return the type of the basic block.
 * PARAMETERS:      <none>
 * RETURNS:         the type of the basic block
 *============================================================================*/
BBTYPE BasicBlock::getType() {
    return m_nodeType;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::updateType
 * OVERVIEW:        Update the type and number of out edges. Used for example
 *                  where a COMPJUMP type is updated to an NWAY when a switch
 *                  idiom is discovered.
 * PARAMETERS:      bbType - the new type
 *                  iNumOutEdges - new number of inedges
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::updateType(BBTYPE bbType, int iNumOutEdges) {
    m_nodeType = bbType;
    m_iNumOutEdges = iNumOutEdges;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::setJumpReqd
 * OVERVIEW:        Sets the "jump required" bit. This means that this BB is
 *                  an orphan (not generated from input code), and that the
 *                  "fall through" out edge needs to be implemented as a jump
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::setJumpReqd() {
    m_bJumpReqd = true;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::isJumpReqd
 * OVERVIEW:        Returns the "jump required" bit. See above for details
 * PARAMETERS:      <none>
 * RETURNS:         True if a jump is required
 *============================================================================*/
bool BasicBlock::isJumpReqd() {
    return m_bJumpReqd;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::prints
 * OVERVIEW:        Print to a static string (for debugging) 
 * PARAMETERS:      <none>
 * RETURNS:         Address of the static buffer
 *============================================================================*/
static char debug_buffer[1000];
char* BasicBlock::prints() {   
    std::ostringstream ost; 
    print(ost);       
    // Static buffer may overflow; that's OK, we just print the first 999 bytes
    strncpy(debug_buffer, ost.str().c_str(), 999);
    debug_buffer[999] = '\0';
    return debug_buffer; 
}

/*==============================================================================
 * FUNCTION:        BasicBlock::print()
 * OVERVIEW:        Display the whole BB to the given stream
 *                  Used for "-R" option, and handy for debugging
 * PARAMETERS:      os - stream to output to
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::print(std::ostream& os, bool withDF) {
    if (m_iLabelNum) os << "L" << std::dec << m_iLabelNum << ": ";
    switch(m_nodeType)
    {
        case ONEWAY:    os << "Oneway BB"; break;
        case TWOWAY:    os << "Twoway BB"; break;
        case NWAY:      os << "Nway BB"; break;
        case CALL:      os << "Call BB"; break;
        case RET:       os << "Ret BB"; break;
        case FALL:      os << "Fall BB"; break;
        case COMPJUMP:  os << "Computed jump BB"; break;
        case COMPCALL:  os << "Computed call BB"; break;
        case INVALID:   os << "Invalid BB"; break;
    }
    // Printing the address is bad for unit testing, since address will
    // be arbitrary
    //os << " (0x" << std::hex << (unsigned int)this << "):\n";
    os << ":";
    if (withDF) {
        os << " live in: ";
        std::set<Statement*> livein;
        getLiveIn(livein);
        for (std::set<Statement*>::iterator it = livein.begin(); it != livein.end(); it++) {
            (*it)->printAsUse(os);
            os << ", ";
        }
    }
    os << std::endl;
    if (m_pRtls)                    // Can be zero if e.g. INVALID
    {
        std::list<RTL*>::iterator rit;
        for (rit = m_pRtls->begin(); rit != m_pRtls->end(); rit++) {
            (*rit)->print(os, withDF);
        }
    }
    if (m_bJumpReqd) {
        os << "Synthetic out edge(s) to ";
        for (int i=0; i < m_iNumOutEdges; i++) {
            PBB outEdge = m_OutEdges[i];
            if (outEdge && outEdge->m_iLabelNum)
                os << "L" << std::dec << outEdge->m_iLabelNum << " ";
        }
        os << std::endl;
    }
}

// These are now the only two user level functions that are in the
// BasicBlock class. Perhaps these will be moved to the Cfg class
// for uniformity, or there will be other reasons - MVE

/*==============================================================================
 * FUNCTION:        BasicBlock::addInterProcOutEdge
 * OVERVIEW:        Adds an interprocedural out-edge to the basic block pBB that
 *                  represents this address.  The local inter-procedural boolean
 *                  is updated as well.  The mapping between addresses and basic
 *                  blocks is done when the  graph is well-formed.
 *                  NOTE: this code was changed to only decrease the # of out
 *                  edges by one so to avoid interprocedural links that are not
 *                  supported by the well-form graph routine.  We need to think
 *                  how to support this in a nice way, so for the moment,
 *                  decrease out-edges works ok.
 * PARAMETERS:      addr -
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::addInterProcOutEdge(ADDRESS addr) {
//  m_OutEdges.push_back((PBB)addr);
//  m_OutEdgeInterProc[m_OutEdges.size()-1] = true;
    m_iNumOutEdges--;
}


/*==============================================================================
 * FUNCTION:        BasicBlock::addProcOutEdge 
 * OVERVIEW:        This function was proposed but never completed
 * PARAMETERS:      uAddr -
 * RETURNS:         <nothing>
 *============================================================================*/
bool BasicBlock::addProcOutEdge (ADDRESS uAddr) {
    // This is a bit problematic. I tried writing one at one stage,
    // and ended up with circular dependencies... Perhaps we don't
    // need this function; you can just use getProcAddr() to find
    // the destintion of the call
    std::cerr << "AddProcOutEdge not implemented yet\n";
    return true;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getLowAddr
 * OVERVIEW:        Get the lowest real address associated with this BB. Note
 *                  that although this is usually the address of the first RTL,
 *                  it is not always so. For example, if the BB contains just a
 *                  delayed branch, and the delay instruction for the branch
 *                  does not affect the branch, so the delay instruction is
 *                  copied in  front of the branch instruction. It's address
 *                  will be UpdateAddress()d to 0, since it is "not really
 *                  there", so the low address for this BB will be the address
 *                  of the branch.
 * PARAMETERS:      <none>
 * RETURNS:         the lowest real address associated with this BB
 *============================================================================*/
ADDRESS BasicBlock::getLowAddr() {
    assert(m_pRtls != NULL);
    ADDRESS a = m_pRtls->front()->getAddress();
    if ((a == 0) && (m_pRtls->size() > 1))
    {
        std::list<RTL*>::iterator it = m_pRtls->begin();
        ADDRESS add2 = (*++it)->getAddress();
        // This is a bit of a hack for 286 programs, whose main actually starts
        // at offset 0. A better solution would be to change orphan BBs'
        // addresses to NO_ADDRESS, but I suspect that this will cause many
        // problems. MVE
        if (add2 < 0x10)
            // Assume that 0 is the real address
            return 0;
        return add2;
    }
    return a;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getHiAddr
 * OVERVIEW:        Get the highest address associated with this BB. This is
 *                  always the address associated with the last RTL.
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
ADDRESS BasicBlock::getHiAddr() {
    assert(m_pRtls != NULL);
    return m_pRtls->back()->getAddress();
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getRTLs
 * OVERVIEW:        Get pointer to the list of RTL*.
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
std::list<RTL*>* BasicBlock::getRTLs() {
    return m_pRtls;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getInEdges
 * OVERVIEW:        Get a constant reference to the vector of in edges.
 * PARAMETERS:      <none>
 * RETURNS:         a constant reference to the vector of in edges
 *============================================================================*/
std::vector<PBB>& BasicBlock::getInEdges() {
    return m_InEdges;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getOutEdges
 * OVERVIEW:        Get a constant reference to the vector of out edges.
 * PARAMETERS:      <none>
 * RETURNS:         a constant reference to the vector of out edges
 *============================================================================*/
std::vector<PBB>& BasicBlock::getOutEdges() {
    return m_OutEdges;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::setInEdge
 * OVERVIEW:        Change the given in-edge (0 is first) to the given value
 *                  Needed for example when duplicating BBs
 * PARAMETERS:      i: index (0 based) of in-edge to change
 *                  pNewInEdge: pointer to BB that will be a new parent
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::setInEdge(int i, PBB pNewInEdge) {
    m_InEdges[i] = pNewInEdge;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::setOutEdge
 * OVERVIEW:        Change the given out-edge (0 is first) to the given value
 *                  Needed for example when duplicating BBs
 * NOTE:            Cannot add an additional out-edge with this function; use
 *                  addOutEdge for this rare case
 * PARAMETERS:      i: index (0 based) of out-edge to change
 *                  pNewOutEdge: pointer to BB that will be the new successor
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::setOutEdge(int i, PBB pNewOutEdge) {
    if (m_OutEdges.size() == 0) {
        assert(i == 0);
        m_OutEdges.push_back(pNewOutEdge);
    } else {
        assert(i < (int)m_OutEdges.size());
        m_OutEdges[i] = pNewOutEdge;
    }
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getOutEdge
 * OVERVIEW:        Returns the i-th out edge of this BB; counting starts at 0
 * NOTE:            
 * PARAMETERS:      i: index (0 based) of the desired out edge
 * RETURNS:         the i-th out edge; 0 if there is no such out edge
 *============================================================================*/
PBB BasicBlock::getOutEdge(unsigned int i) {
    if (i < m_OutEdges.size()) return m_OutEdges[i];
    else return 0;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getCorrectOutEdge
 * OVERVIEW:        given an address this method returns the corresponding
 *                  out edge
 * NOTE:            
 * PARAMETERS:      a: the address
 * RETURNS:         
 *============================================================================*/

PBB BasicBlock::getCorrectOutEdge(ADDRESS a) {
    std::vector<PBB>::iterator it;
    for (it = m_OutEdges.begin(); it != m_OutEdges.end(); it++) {
        if ((*it)->getLowAddr() == a) return *it; 
    }
    return 0;
}


/*==============================================================================
 * FUNCTION:        BasicBlock::addInEdge
 * OVERVIEW:        Add the given in-edge
 *                  Needed for example when duplicating BBs
 * PARAMETERS:      pNewInEdge: pointer to BB that will be a new parent
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::addInEdge(PBB pNewInEdge) {
    m_InEdges.push_back(pNewInEdge);
    m_iNumInEdges++;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::deleteInEdge
 * OVERVIEW:        Delete the in-edge from the given BB
 *                  Needed for example when duplicating BBs
 * PARAMETERS:      it: iterator to BB that will no longer be a parent
 * SIDE EFFECTS:    The iterator argument is incremented.
 * USAGE:           It should be used like this:
 *                    if (pred) deleteInEdge(it) else it++;
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::deleteInEdge(std::vector<PBB>::iterator& it) {
    it = m_InEdges.erase(it);
    m_iNumInEdges--;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getCoverage
 * OVERVIEW:        
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
unsigned BasicBlock::getCoverage() {
    assert(m_pRtls != NULL);
    unsigned uTotal = 0;
    std::list<RTL*>::iterator it;
    for (it = m_pRtls->begin(); it != m_pRtls->end(); it++) {
        if ((*it)->getAddress())
            uTotal += (unsigned)(*it)->getNumBytes();
    }
    return uTotal;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::DFTOrder
 * OVERVIEW:        Traverse this node and recurse on its children in a depth
 *                  first manner. Records the times at which this node was first
 *                  visited and last visited
 * PARAMETERS:      first - the number of nodes that have been visited
 *                  last - the number of nodes that have been visited for the
 *                    last time during this traversal
 * RETURNS:         the number of nodes (including this one) that were traversed
 *                  from this node
 *============================================================================*/
unsigned BasicBlock::DFTOrder(int& first, int& last) {
    first++;
    m_DFTfirst = first;

    unsigned numTraversed = 1;
    m_iTraversed = true;

    for (std::vector<PBB>::iterator it = m_OutEdges.begin();
      it != m_OutEdges.end(); it++) {

        PBB child = *it;
        if (child->m_iTraversed == false)
            numTraversed = numTraversed + child->DFTOrder(first,last);
    }

    last++;
    m_DFTlast = last;

    return numTraversed;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::DFTOrder
 * OVERVIEW:        Traverse this node and recurse on its parents in a reverse 
 *          depth first manner. Records the times at which this node was 
 *          first visited and last visited
 * PARAMETERS:      first - the number of nodes that have been visited
 *                  last - the number of nodes that have been visited for the
 *                    last time during this traversal
 * RETURNS:         the number of nodes (including this one) that were traversed
 *                  from this node
 *============================================================================*/
unsigned BasicBlock::RevDFTOrder(int& first, int& last) {
    first++;
    m_DFTrevfirst = first;

    unsigned numTraversed = 1;
    m_iTraversed = true;

    for (std::vector<PBB>::iterator it = m_InEdges.begin(); it != m_InEdges.end();
         it++) {

        PBB parent = *it;
        if (parent->m_iTraversed == false)
            numTraversed = numTraversed + parent->RevDFTOrder(first,last);
    }

    last++;
    m_DFTrevlast = last;

    return numTraversed;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::lessAddress
 * OVERVIEW:        Static comparison function that returns true if the first BB
 *                  has an address less than the second BB.
 * PARAMETERS:      bb1 - first BB
 *                  bb2 - last BB
 * RETURNS:         bb1.address < bb2.address
 *============================================================================*/
bool BasicBlock::lessAddress(PBB bb1, PBB bb2) {
    return bb1->getLowAddr() < bb2->getLowAddr();
}

/*==============================================================================
 * FUNCTION:        BasicBlock::lessFirstDFT
 * OVERVIEW:        Static comparison function that returns true if the first BB
 *                  has DFT first order less than the second BB.
 * PARAMETERS:      bb1 - first BB
 *                  bb2 - last BB
 * RETURNS:         bb1.first_DFS < bb2.first_DFS
 *============================================================================*/
bool BasicBlock::lessFirstDFT(PBB bb1, PBB bb2) {
    return bb1->m_DFTfirst < bb2->m_DFTfirst;
}


#if 0
/*==============================================================================
 * FUNCTION:        BasicBlock::resetDFASets
 * OVERVIEW:        Resets the DFA sets of this BB.
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void BasicBlock::resetDFASets() {
    liveOut.reset();
    defSet.reset();
    useSet.reset();
    useUndefSet.reset();
    recursiveUseUndefSet.reset();
}
#endif

/*==============================================================================
 * FUNCTION:        BasicBlock::lessLastDFT
 * OVERVIEW:        Static comparison function that returns true if the first BB
 *                  has DFT first order less than the second BB.
 * PARAMETERS:      bb1 - first BB
 *                  bb2 - last BB
 * RETURNS:         bb1.last_DFS < bb2.last_DFS
 *============================================================================*/
bool BasicBlock::lessLastDFT(PBB bb1, PBB bb2) {
    return bb1->m_DFTlast < bb2->m_DFTlast;
}

/*==============================================================================
 * FUNCTION:        BasicBlock::getCallDest
 * OVERVIEW:        Get the destination of the call, if this is a CALL BB with
 *                    a fixed dest. Otherwise, return -1
 * PARAMETERS:      <none>
 * RETURNS:         Native destination of the call, or -1
 *============================================================================*/
ADDRESS BasicBlock::getCallDest() {
    if (m_nodeType != CALL)
        return (ADDRESS)-1;
    std::list<RTL*>::reverse_iterator it;
    for (it = m_pRtls->rbegin(); it != m_pRtls->rend(); it++) {
        if ((*it)->getKind() == CALL_RTL)
            return ((HLCall*)(*it))->getFixedDest();
    }
    return (ADDRESS)-1;
}

// serialize this BB
bool BasicBlock::serialize(std::ostream &ouf, int &len) {
    std::streampos st = ouf.tellp();

    saveFID(ouf, FID_BB_TYPE);
    saveValue(ouf, (char)m_nodeType);

    int ncount = 0;
    for (std::vector<PBB>::iterator it = m_OutEdges.begin(); it != m_OutEdges.end(); it++) 
        ncount++;
    if (ncount) {
        saveFID(ouf, FID_BB_OUTEDGES);
        saveLen(ouf, ncount * 4);
        saveValue(ouf, ncount, false);
        for (
#ifndef WIN32
            std::vector<PBB>::iterator 
#endif
            it = m_OutEdges.begin(); it != m_OutEdges.end(); it++) {
            saveValue(ouf, (*it)->m_nindex, false);
        }
    }

    if (m_pRtls) {
        for (std::list<RTL*>::iterator it = m_pRtls->begin(); it != m_pRtls->end(); it++) {
            saveFID(ouf, FID_BB_RTL);
            std::streampos pos = ouf.tellp();
            int len = -1;
            saveLen(ouf, -1, true);
            std::streampos posa = ouf.tellp();

            assert((*it)->serialize(ouf, len));

            std::streampos now = ouf.tellp();
            assert((int)(now - posa) == len);
            ouf.seekp(pos);
            saveLen(ouf, len, true);
            ouf.seekp(now);
        }
    }

    saveFID(ouf, FID_BB_END);
    saveLen(ouf, 0);

    len = ouf.tellp() - st;
    return true;
}

// deserialize a BB
bool BasicBlock::deserialize(std::istream &inf) {
    int fid;
    int i, l, j;
    char ch;

    m_nOutEdges.clear();

    while ((fid = loadFID(inf)) != -1 && fid != FID_BB_END) {
        switch (fid) {
            case FID_BB_TYPE:
                loadValue(inf, ch);
                assert(ch <= INVALID);
                m_nodeType = (BBTYPE)ch;
                break;
            case FID_BB_OUTEDGES:
                l = loadLen(inf);
                loadValue(inf, i, false);
                assert(l == i * 4);
                for (j = 0; j < i; j++) {
                    int o;
                    loadValue(inf, o, false);
                    m_nOutEdges.push_back(o);
                }
                break;
            case FID_BB_RTL:
                {
                    int len = loadLen(inf);
                    std::streampos pos = inf.tellg();
                    RTL *rtl = RTL::deserialize(inf);
                    if (rtl) {
                        assert((int)(inf.tellg() - pos) == len);
                        if (m_pRtls == NULL) m_pRtls = new std::list<RTL*>();
                        assert(m_pRtls);
                        m_pRtls->push_back(rtl);
                    } else {
                        // unknown rtl type, skip it
                        inf.seekg(pos + (std::streamoff)len);
                    }
                }
                break;
    /*
    TODO:

    std::list<RTL*>*     m_pRtls;        // Ptr to list of RTLs
    int             m_iLabelNum;    // Nonzero if start of BB needs label
    bool            m_bIncomplete:1;// True if not yet complete
    bool            m_bJumpReqd:1;  // True if jump required for "fall through"
    
    std::vector<PBB>     m_InEdges;      // Vector of in-edges
    std::vector<PBB>     m_OutEdges;     // Vector of out-edges
    int             m_iNumInEdges;  // We need these two because GCC doesn't

    bool            m_iTraversed;   // traversal marker

    Exp* returnLoc;

    */
            default:
                skipFID(inf, fid);
        }
    }
    assert(loadLen(inf) == 0);

    return true;
}

/*
 * Structuring and code generation.
 *
 * This code is whole heartly based on AST by Doug Simon.  Portions may be
 * copyright to him and are available under a BSD style license.  
 *
 * Adapted for Boomerang by Trent Waddington, 20 June 2002.
 *
 */

/* Get the condition */
Exp *BasicBlock::getCond() {
    // the condition will be in the last rtl
    assert(m_pRtls);
    RTL *last = m_pRtls->back();
    // it should be a HLJcond
    assert(last->getKind() == JCOND_RTL);
    HLJcond *j = (HLJcond*)last;
    Exp *e = j->getCondExpr();  
    return e;
}

void BasicBlock::setCond(Exp *e) {
    // the condition will be in the last rtl
    assert(m_pRtls);
    RTL *last = m_pRtls->back();
    // it should be a HLJcond
    assert(last->getKind() == JCOND_RTL);
    HLJcond *j = (HLJcond*)last;
    j->setCondExpr(e);
}

/* Check for branch if equal relation */
bool BasicBlock::isJmpZ(PBB dest) {
    // The condition will be in the last rtl
    assert(m_pRtls);
    RTL *last = m_pRtls->back();
    // It should be a HLJcond
    assert(last->getKind() == JCOND_RTL);
    HLJcond *j = (HLJcond*)last;
    JCOND_TYPE jt = j->getCond();
    if ((jt != HLJCOND_JE) && (jt != HLJCOND_JNE)) return false;
    PBB trueEdge = m_OutEdges[0];
    if (jt == HLJCOND_JE)
        return dest == trueEdge;
    else {
        PBB falseEdge = m_OutEdges[1];
        return dest == falseEdge;
    }
}

/* Get the loop body */
BasicBlock *BasicBlock::getLoopBody() {
    assert(m_structType == PRETESTLOOP || m_structType == POSTTESTLOOP ||
      m_structType == ENDLESSLOOP);
    assert(m_iNumOutEdges == 2);
    if (m_OutEdges[0] != m_loopFollow)
        return m_OutEdges[0];
    return m_OutEdges[1];
}

bool BasicBlock::isAncestorOf(BasicBlock *other) {           
    return ((loopStamps[0] < other->loopStamps[0] && 
             loopStamps[1] > other->loopStamps[1]) ||
            (revLoopStamps[0] < other->revLoopStamps[0] && 
             revLoopStamps[1] > other->revLoopStamps[1]));
/*    return (m_DFTfirst < other->m_DFTfirst && m_DFTlast > other->m_DFTlast) ||
      (m_DFTrevlast < other->m_DFTrevlast &&
       m_DFTrevfirst > other->m_DFTrevfirst);*/
}

void BasicBlock::simplify() {
    for (std::list<RTL*>::iterator it = m_pRtls->begin();
      it != m_pRtls->end(); it++)
        (*it)->simplify();
}
        
bool BasicBlock::hasBackEdgeTo(BasicBlock* dest) {
//  assert(HasEdgeTo(dest) || dest == this);
    return dest == this || dest->isAncestorOf(this);
}

bool BasicBlock::allParentsTraversed() {
    for (int i = 0; i < m_iNumInEdges; i++)
        if (!m_InEdges[i]->m_iTraversed && m_InEdges[i]->hasBackEdgeTo(this))
            return false;
    return true;
}

// generate code for the body of this BB
void BasicBlock::generateBodyCode(HLLCode &hll, bool dup) {
    if (!dup)
        hll.AddLabel(this); 
    if (m_pRtls)
        for (std::list<RTL*>::iterator it = m_pRtls->begin();
          it != m_pRtls->end(); it++) 
            (*it)->generateCode(hll, this);
}


// Generate code for this BB, belonging to the loop specified by latch
// (initially NULL) 
void BasicBlock::generateCode(HLLCode &hll, BasicBlock *latch, bool loopCond) {
    if (hll.gotoSetContains(this) && !isLatchNode() && 
      ((latch && this == latch->m_loopHead->m_loopFollow) ||
        !allParentsTraversed())
    ) {
        hll.AddGoto(this, this);
        return;
    } else if (hll.followSetContains(this)) {
        if (this != hll.getEnclFollow()) {
            hll.AddGoto(this, this);
        }
        return;
    }

    if (m_iTraversed) {
//      assert(m_sType == Loop && m_lType == PostTested && m_latchNode == this);
        return;
    }
    m_iTraversed = true;

    if (isLatchNode()) {
        // Some stuff about indentation level.. this is a little too language
        // specific, so I need to abstract this - trent.
        generateBodyCode(hll); // or generate a goto
        return;
    }

    if (m_structType == NONE && m_iNumOutEdges > 1) {
        // we know it has to be a conditional
        m_structType = IFGOTO;
        m_condFollow = m_OutEdges[1];
    }

    switch(loopCond ? m_loopCondType : m_structType) {
        case PRETESTLOOP:
            // add the follow of the loop (if it exists) to the follow set
            if (m_loopFollow)
                hll.addtoFollowSet(m_loopFollow);

            assert(m_latchNode->m_OutEdges.size() == 1);
            // add anything before the loop test
            generateBodyCode(hll);
            // add the header
            hll.AddPretestedLoopHeader(this, getCond());
            // add the code for the body of the loop
            getLoopBody()->generateCode(hll, m_latchNode);
            // add the code for the latch if not already added
            if (!m_latchNode->isTraversed()) {
                m_latchNode->setTraversed(true);
                m_latchNode->generateBodyCode(hll);
            }
            // add the stuff before the loop again (code duplication, yum)
            generateBodyCode(hll, true);
            // close the loop
            hll.AddPretestedLoopEnd(this);

            // write the code for the follow of the loop (if it exists)
            if (m_loopFollow) {
                // remove the follow from the follow set
                hll.removeLastFollow();

                if (!m_loopFollow->isTraversed())
                    m_loopFollow->generateCode(hll, latch);
                else
                    hll.AddGoto(this, m_loopFollow);
            }
            break;
        case POSTTESTLOOP:
            // add the follow of the loop (if it exists) to the follow set
            if (m_loopFollow)
                hll.addtoFollowSet(m_loopFollow);

            // add the header
            hll.AddPosttestedLoopHeader(this);
            // ensure there is a conditional if more than one out edge
            if (m_OutEdges.size() == 2 && m_loopCondType == NONE) {
                m_loopCondType = IFGOTO;
            }
            // add code for the conditional (if any)
            if (m_loopCondType != NONE) {
                // a conditional inside a loop is a pain
                setTraversed(false);
                generateCode(hll, m_latchNode, true);
            } else {
                // add code for loop body
                generateBodyCode(hll);
                getLoopBody()->generateCode(hll, m_latchNode);
            }

            // add the code for the latch if not already added
            if (!m_latchNode->isTraversed()) {
                m_latchNode->setTraversed(true);
                m_latchNode->generateBodyCode(hll);
            }
            // close the loop
            hll.AddPosttestedLoopEnd(this, getCond());              

            // write the code for the follow of the loop (if it exists)
            if (m_loopFollow) {
                // remove the follow from the follow set
                hll.removeLastFollow();

                if (!m_loopFollow->isTraversed())
                    m_loopFollow->generateCode(hll, latch);
                else
                    hll.AddGoto(this, m_loopFollow);
            }
            break;
        case ENDLESSLOOP:
            // add the follow of the loop (if it exists) to the follow set
            if (m_loopFollow)
                hll.addtoFollowSet(m_loopFollow);
            // add the header
            hll.AddEndlessLoopHeader(this);
            // add code for the conditional (if any)
            if (m_loopCondType != NONE) {
                // a conditional inside a loop is a pain
                setTraversed(false);
                generateCode(hll, m_latchNode, true);
            } else {
                // add code for loop body
                generateBodyCode(hll);
                getLoopBody()->generateCode(hll, m_latchNode);
            }
            // add the code for the latch if not already added
            if (!m_latchNode->isTraversed()) {
                m_latchNode->setTraversed(true);
                m_latchNode->generateBodyCode(hll);
            }
            // close the loop
            hll.AddEndlessLoopEnd(this);

            // write the code for the follow of the loop (if it exists)
            if (m_loopFollow) {
                // remove the follow from the follow set
                hll.removeLastFollow();

                if (!m_loopFollow->isTraversed())
                    m_loopFollow->generateCode(hll, latch);
                else
                    hll.AddGoto(this, m_loopFollow);
            }
            break;
        case JUMPINOUTLOOP:
            break;
        case JUMPINTOCASE:
            break;
        case IFGOTO:
            // add code for the header
            generateBodyCode(hll);
            // add the if header
            hll.AddIfCondHeader(this, getCond());
            // ensure that the follow is one of the out edges
            if (m_condFollow != m_OutEdges[0] && m_condFollow != m_OutEdges[1])
                // Hopefully the condition hasn't been incorrectly flipped.
                m_condFollow = m_OutEdges[1];
            // ensure out edge 0 is not the follow
            if (m_condFollow == m_OutEdges[0]) {
                // swap the condition and the out edges
                Exp *e = new Unary(opLNot, getCond()->clone());
                e = e->simplify();
                setCond(e);
                PBB tmp = m_OutEdges[0];
                m_OutEdges[0] = m_OutEdges[1];
                m_OutEdges[1] = tmp;
            }
            // add a goto for the body
            hll.AddGoto(this, m_OutEdges[0]);
            // add if end
            hll.AddIfCondEnd(this);
            // add code for the follow
            m_condFollow->generateCode(hll, latch);
            break;
        case IFTHEN:
            // add code for the header
            generateBodyCode(hll);      
            // ensure out edge 0 is not the follow
            if (m_condFollow == m_OutEdges[0]) {
                // swap the condition and the out edges
                Exp *e = new Unary(opLNot, getCond()->clone());
                e = e->simplify();
                setCond(e);
                PBB tmp = m_OutEdges[0];
                m_OutEdges[0] = m_OutEdges[1];
                m_OutEdges[1] = tmp;
            }
            // add the if header
            hll.AddIfCondHeader(this, getCond());
            // add code or goto for the body of the if
            hll.addtoFollowSet(m_condFollow);
            assert(m_condFollow != m_OutEdges[0]);
            if (m_OutEdges[0]->isTraversed() ||
              (m_loopHead && m_OutEdges[0] == m_loopHead->m_loopFollow))
                hll.AddGoto(this, m_OutEdges[0]);
            else {
                m_OutEdges[0]->generateCode(hll, latch);
            }
            // add if end
            hll.AddIfCondEnd(this);
            // remove the follow
            assert(hll.getEnclFollow() == m_condFollow);
            hll.removeLastFollow();
            // add code for the follow
            m_condFollow->generateCode(hll, latch);
            break;
        case IFTHENELSE:
            // add code for the header
            generateBodyCode(hll);
            // add the if header
            hll.AddIfElseCondHeader(this, getCond());
            // add code or goto for the then body of the if
            assert(m_condFollow != m_OutEdges[0]);
            hll.addtoFollowSet(m_condFollow);
            if (m_OutEdges[0]->isTraversed() ||
              (m_loopHead && m_OutEdges[0] == m_loopHead->m_loopFollow))
                hll.AddGoto(this, m_OutEdges[0]);
            else {
                m_OutEdges[0]->generateCode(hll, latch);
            }
            // add if else
            hll.AddIfElseCondOption(this);
            // add code or goto for the then body of the if
            assert(m_condFollow != m_OutEdges[1]);
            if (m_OutEdges[1]->isTraversed() ||
              (m_loopHead && m_OutEdges[1] == m_loopHead->m_loopFollow))
                hll.AddGoto(this, m_OutEdges[1]);
            else {
                m_OutEdges[1]->generateCode(hll, latch);
            }
            // add if end
            hll.AddIfElseCondEnd(this);
            // remove the follow
            assert(hll.getEnclFollow() == m_condFollow);
            hll.removeLastFollow();
            // add code for the follow
            m_condFollow->generateCode(hll, latch);
            break;
        case IFELSE:
            assert(false);
            break;
        case CASE:
            assert(false);
            hll.addtoFollowSet(m_condFollow);

            break;
        case NONE:
            // add code for the body
            generateBodyCode(hll);

            // generate a ret if this is one
            if (m_nodeType == RET) {
                hll.AddReturnStatement(this, m_returnVal);
                return;
            }

            // get child
            BasicBlock *child = getOutEdge(0);
            if (!child) return;

            // if the child has already been traversed
            if (child->isTraversed()) {
                hll.AddGoto(this, child);
                return;
            }

            // or the child is the latch
            if (latch && latch->m_loopHead->m_loopFollow == child) {
                hll.AddGoto(this, child);
                return;
            }

            // or the child is not in the same loop
            if (child->m_loopHead != m_loopHead) {
                // but has more than one untraversed parent or is a follow
                if (!child->allParentsTraversed() ||
                  hll.followSetContains(child)) {
                    hll.AddGoto(this, child);
                    return;
                }
            }

            // or the child is not in the same case
            if (child->m_caseHead != m_caseHead) {
                hll.AddGoto(this, child);
                return;
            }

            // or the child is the follow of this case, add a goto
            if (m_caseHead && child == m_caseHead->m_condFollow) {
                hll.AddGoto(this, child);
                return;
            }

            // otherwise, add the code
            child->generateCode(hll, latch);
            
            break;
    }   
}

// some constants required to indicate extra code to be added
#define CLOSE_BRACKET 2		// length of "}\n"
#define DO_HEADER 5			// length of "do {\n"
#define ENDLESS_HEADER 11	// langth of "for (;;) {\n"

// Return the string containing indLevel tabstops. 
// If extra = CLOSE_BRACKET then append "}\n" to the returned string
// If extra = DO_HEADER then append "do {\n" to the returned string
char* BasicBlock::indent(int indLevel, int extra/* = 0*/)
{
    char* retStr = new char[indLevel + extra + 1];
    memset(retStr,'\t',indLevel);

    retStr[indLevel] = '\0';

    // add the extra stuff in necessary
    if (extra == CLOSE_BRACKET)
        strcat(retStr,"}\n");
    else if (extra == DO_HEADER)
        strcat(retStr,"do {\n");
    else if (extra == ENDLESS_HEADER)
        strcat(retStr,"for (;;) {\n");

    return retStr;
}

// Return true if every parent (i.e. forward in edge source) of this node has 
// had its code generated
bool BasicBlock::allParentsGenerated()
{
    for (unsigned int i = 0; i < m_InEdges.size(); i++)
        if (!m_InEdges[i]->hasBackEdgeTo(this) && 
            m_InEdges[i]->traversed != DFS_CODEGEN)
            return false;
    return true;
}

// Emits a goto statement (at the correct indentation level) with the 
// destination label for dest. Also places the label just before the 
// destination code if it isn't already there.  If the goto is to the return 
// block, emit a 'return' instead.  Also, 'continue' and 'break' statements 
// are used instead if possible
void BasicBlock::emitGotoAndLabel(std::list<char*> &lines, int indLevel, 
    PBB dest)
{
    // is this a goto to the ret block?
    if (dest->getType() == RET) { // WAS: check about size of ret bb
        char* retStmt = new char[indLevel * 2 + strlen("return;\n\n") + 1];

        sprintf(retStmt, "%sreturn;\n", indent(indLevel));
	lines.push_back(retStmt);
    } else { 
        char* gotoStmt;

        if (loopHead && (loopHead == dest || loopHead->loopFollow == dest)) {
            gotoStmt = new char[indLevel + strlen("continue;\n") + 1];
            sprintf(gotoStmt, "%s%s\n", indent(indLevel),
                    (loopHead == dest ? "continue;" : "break;"));
        } else {
            gotoStmt = new char[indLevel + strlen("goto L;\n") + 
                                1024 + 1];
            sprintf(gotoStmt, "%sgoto L%d;\n", indent(indLevel), dest->ord);

            // don't emit the label if it already has been emitted or the code 
            // for the destination has not yet been generated
            if (!dest->hllLabel && dest->traversed == DFS_CODEGEN)
                sprintf(dest->labelStr,"L%d:\n",dest->ord);

            dest->hllLabel = true;
        }
        lines.push_back(gotoStmt);
    }
}

// Generates code for each non CTI (except procedure calls) statement within 
// the block.
void BasicBlock::WriteBB(std::list<char*> &lines, int indLevel)
{
    // allocate space for a label to be generated for this node and add this to
    // the generated code. The actual label can then be generated now or back 
    // patched later
    labelStr = new char[1024];
    lines.push_back(labelStr);
    if (hllLabel)
	sprintf(labelStr,"L%d:\n",ord);
    else
        strcpy(labelStr,"");

    lines.push_back("BB\n");
	
    // save the indentation level that this node was written at
    indentLevel = indLevel;
}

void BasicBlock::generateCode(std::list<char*> &lines, int indLevel, 
    PBB latch, std::list<PBB> &followSet, std::list<PBB> &gotoSet)
{
    // If this is the follow for the most nested enclosing conditional, then
    // don't generate anything. Otherwise if it is in the follow set
    // generate a goto to the follow
    PBB enclFollow = followSet.size() == 0 ? NULL : 
                     followSet.back();

    if (isIn(gotoSet, this) && !isLatchNode() && 
        ((latch && this == latch->loopHead->loopFollow) || 
        !allParentsGenerated())) {
        emitGotoAndLabel(lines, indLevel, this);
        return;
    } else if (isIn(followSet, this)) {
        if (this != enclFollow) {
            emitGotoAndLabel(lines, indLevel, this);
            return;
        } else return;
    }

    // Has this node already been generated?
    if (traversed == DFS_CODEGEN) {
        // this should only occur for a loop over a single block
        assert(sType == Loop && lType == PostTested && latchNode == this);
        return;
    } else
        traversed = DFS_CODEGEN;

    // if this is a latchNode and the current indentation level is
    // the same as the first node in the loop, then this write out its body 
    // and return otherwise generate a goto
    if (isLatchNode())
        if (indLevel == latch->loopHead->indentLevel + 
                        (latch->loopHead->lType == PreTested ? 1 : 0)) {
            WriteBB(lines, indLevel);
            return;
        } else {
	    // unset its traversed flag
	    traversed = UNTRAVERSED;

	    emitGotoAndLabel(lines, indLevel, this);
	    return;
        }
	
    // declare some strings that can't be initialised in the body of the switch
    char* condPred;
    char* predString;
    char* opCode;
    char* retStmt;

    switch(sType) {
        case Loop:
        case LoopCond:
            // add the follow of the loop (if it exists) to the follow set
            if (loopFollow)
                followSet.push_back(loopFollow);

            if (lType == PreTested) {
                assert(latchNode->m_OutEdges.size() == 1);

                // write the body of the block (excluding the predicate)
                WriteBB(lines, indLevel);

                // write the 'while' predicate
                Exp *cond = getCond();
                if (m_OutEdges[BTHEN] == loopFollow) {
                    cond = new Unary(opNot, cond);
                    cond = cond->simplify();
                }
                std::ostringstream str;
                cond->print(str);
                opCode = (char*)str.str().c_str();
                predString = new char[(indLevel * 2) + 1 + strlen("while (") + 
                                      1024 + strlen(")\n") + 
                                      strlen("{\n") + 1];
                sprintf(predString, "%swhile (%s)\n%s{\n", 
                        indent(indLevel), opCode, indent(indLevel));
                lines.push_back(predString);

		// write the code for the body of the loop
		PBB loopBody = (m_OutEdges[BELSE] == loopFollow) ? 
                                m_OutEdges[BTHEN] : m_OutEdges[BELSE];
                loopBody->generateCode(lines, indLevel + 1, latchNode, 
                    followSet, gotoSet);

                // if code has not been generated for the latch node, generate 
                // it now
                if (latchNode->traversed != DFS_CODEGEN) {
                    latchNode->traversed = DFS_CODEGEN;
                    latchNode->WriteBB(lines, indLevel+1);
                }

                // rewrite the body of the block (excluding the predicate) at 
                // the next nesting level after making sure another label 
                // won't be generated
                hllLabel = false;
                WriteBB(lines, indLevel+1);

                // write the loop tail
                lines.push_back(indent(indLevel, CLOSE_BRACKET));
            } else {
                // write the loop header
                if (lType == Endless)
                    lines.push_back(indent(indLevel,ENDLESS_HEADER));
                else
                    lines.push_back(indent(indLevel,DO_HEADER));

                // if this is also a conditional header, then generate code 
                // for the conditional. Otherwise generate code for the loop 
                // body.
                if (sType == LoopCond) {
                    // set the necessary flags so that generateCode can 
                    // successfully be called again on this node
                    sType = Cond;
                    traversed = UNTRAVERSED;
                    generateCode(lines, indLevel + 1, latchNode, followSet, 
                                 gotoSet);
                } else {
                    WriteBB(lines, indLevel+1);

                    // write the code for the body of the loop
                    m_OutEdges[0]->generateCode(lines, indLevel + 1, latchNode, 
                                             followSet, gotoSet);
                }

                if (lType == PostTested) {
                    // if code has not been generated for the latch node, 
                    // generate it now
                    if (latchNode->traversed != DFS_CODEGEN) {
                        latchNode->traversed = DFS_CODEGEN;
                        latchNode->WriteBB(lines, indLevel+1);
                    }
			
                    // string for the repeat loop predicate.
                    predString = new char[indLevel + strlen("} while (") + 
                                          1024 + strlen(")\n") + 2];

                    // write the repeat loop predicate
                    std::ostringstream str;
                    getCond()->print(str);
                    opCode = (char*)str.str().c_str();
                    sprintf(predString,"%s} while (%s);\n", indent(indLevel),
                            opCode);
                    lines.push_back(predString);
                } else {
                    assert(lType == Endless);

                    // if code has not been generated for the latch node, 
                    // generate it now
                    if (latchNode->traversed != DFS_CODEGEN) {
                        latchNode->traversed = DFS_CODEGEN;
                        latchNode->WriteBB(lines, indLevel+1);
                    }

                    // write the closing bracket for an endless loop
                    lines.push_back(indent(indLevel,CLOSE_BRACKET));
                }
            }

            // write the code for the follow of the loop (if it exists)
            if (loopFollow) {
                // remove the follow from the follow set
                followSet.resize(followSet.size()-1);

                if (loopFollow->traversed != DFS_CODEGEN)
                    loopFollow->generateCode(lines, indLevel, latch, followSet,
                                             gotoSet);
                else
                    emitGotoAndLabel(lines, indLevel, loopFollow);
            }
            break;

        case Cond:
        {
            // reset this back to LoopCond if it was originally of this type
            if (latchNode)
                sType = LoopCond;

            // for 2 way conditional headers that are effectively jumps into 
            // or out of a loop or case body, we will need a new follow node
            PBB tmpCondFollow = NULL;

            // keep track of how many nodes were added to the goto set so that 
            // the correct number are removed
            int gotoTotal = 0;

            // add the follow to the follow set if this is a case header
            if (cType == Case)
                followSet.push_back(condFollow);
            else if (cType != Case && condFollow) {
                // For a structured two conditional header, its follow is 
                // added to the follow set
                PBB myLoopHead = (sType == LoopCond ? this : loopHead);

                if (usType == Structured)
                    followSet.push_back(condFollow);
	
                // Otherwise, for a jump into/outof a loop body, the follow is 
                // added to the goto set.  The temporary follow is set for any 
                // unstructured conditional header branch that is within the 
                // same loop and case.
		else {
		    if (usType == JumpInOutLoop) {
		        // define the loop header to be compared against
			PBB myLoopHead = (sType == LoopCond ? this : loopHead);
                        gotoSet.push_back(condFollow);
                        gotoTotal++;
	
                        // also add the current latch node, and the loop header
                        // of the follow if they exist
                        if (latch) {
                            gotoSet.push_back(latch);
                            gotoTotal++;
                        }
                        
                        if (condFollow->loopHead && 
                            condFollow->loopHead != myLoopHead) {
                            gotoSet.push_back(condFollow->loopHead);
                            gotoTotal++;
                        }
                    }

                    if (cType == IfThen)
                        tmpCondFollow = m_OutEdges[BELSE];
                    else
                        tmpCondFollow = m_OutEdges[BTHEN];

                    // for a jump into a case, the temp follow is added to the 
                    // follow set
                    if (usType == JumpIntoCase)
                        followSet.push_back(tmpCondFollow);
                }
            }

            // write the body of the block (excluding the predicate)
            WriteBB(lines, indLevel);

            // write the conditional header 
            if (cType == Case) {
                condPred = new char[indLevel * 2 + strlen("switch (!") + 
                                    1024 + 4 + 1];
                sprintf(condPred, "%sswitch (%s) {\n", indent(indLevel), "Reg0");
            } else {
                Exp *cond = getCond();
                if (cType == IfElse) {
                    cond = new Unary(opNot, cond);
                    cond = cond->simplify();
                }
                std::ostringstream str;
                cond->print(str);
                opCode = (char*)str.str().c_str();
                condPred = new char[indLevel * 2 + strlen("if (") + 
                                    1024 + 4 + 1];
                sprintf(condPred, "%sif (%s) {\n", indent(indLevel), opCode);
            }
            lines.push_back(condPred);

            // write code for the body of the conditional
            if (cType != Case) {
                PBB succ = (cType == IfElse ? m_OutEdges[BELSE] : 
                    m_OutEdges[BTHEN]);

                // emit a goto statement if the first clause has already been 
                // generated or it is the follow of this node's enclosing loop
                if (succ->traversed == DFS_CODEGEN || 
                    (loopHead && succ == loopHead->loopFollow))
                    emitGotoAndLabel(lines, indLevel + 1, succ);
                else	
                    succ->generateCode(lines, indLevel + 1, latch, followSet, 
                                    gotoSet);

                // generate the else clause if necessary
                if (cType == IfThenElse) {
                    // generate the 'else' keyword and matching brackets
                    char* elseStr = new char[indLevel * 2 + 
                                             strlen("} else\n{\n") + 1];
                    sprintf(elseStr, "%s} else\n%s{\n", indent(indLevel),
                            indent(indLevel));
                    lines.push_back(elseStr);

                    succ = m_OutEdges[BELSE];

                    // emit a goto statement if the second clause has already 
                    // been generated
                    if (succ->traversed == DFS_CODEGEN)
                        emitGotoAndLabel(lines, indLevel + 1, succ);
                    else
                        succ->generateCode(lines, indLevel + 1, latch, 
                                        followSet, gotoSet);
                }	
            } else { // case header
                // generate code for each out branch
                for (unsigned int i = 0; i < m_OutEdges.size(); i++) {
                    // emit a case label
                    char* caseStr = new char[indLevel + strlen("cond_:\n") + 
                          1024 + 1];
                    sprintf(caseStr, "%scase cond_%d:\n", indent(indLevel), i);
                    lines.push_back(caseStr);

                    // generate code for the current outedge
                    PBB succ = m_OutEdges[i];
//assert(succ->caseHead == this || succ == condFollow || HasBackEdgeTo(succ));
                    if (succ->traversed == DFS_CODEGEN)
                        emitGotoAndLabel(lines, indLevel + 1, succ);
                    else
                        succ->generateCode(lines, indLevel + 1, latch, 
                            followSet, gotoSet);

                    // generate the 'break' statement
                    caseStr = new char[indLevel + 1 + strlen("break;\n") + 1];
                    sprintf(caseStr,"%sbreak;\n",indent(indLevel + 1));
                    lines.push_back(caseStr);
                }
            }

            // generate the closing bracket
            lines.push_back(indent(indLevel,CLOSE_BRACKET));

            // do all the follow stuff if this conditional had one
            if (condFollow) {
                // remove the original follow from the follow set if it was 
                // added by this header
                if (usType == Structured || usType == JumpIntoCase) {
                    assert(gotoTotal == 0);
                    followSet.resize(followSet.size()-1);
                } else // remove all the nodes added to the goto set
                    for (int i = 0; i < gotoTotal; i++)
                        gotoSet.resize(gotoSet.size()-1);

                // do the code generation (or goto emitting) for the new 
                // conditional follow if it exists, otherwise do it for the 
                // original follow
                if (!tmpCondFollow)
                    tmpCondFollow = condFollow;
			
                if (tmpCondFollow->traversed == DFS_CODEGEN)
                    emitGotoAndLabel(lines, indLevel, tmpCondFollow);
                else
                    tmpCondFollow->generateCode(lines, indLevel, latch,
                        followSet, gotoSet);
            }
            break;
	} 
	case Seq:
            // generate code for the body of this block
            WriteBB(lines, indLevel);

            // return if this is the 'return' block (i.e. has no out edges)
            // after emmitting a 'return' statement
            if (getType() == RET) {
                retStmt = new char[indLevel * 2 + strlen("return;\n") + 1];
                sprintf(retStmt, "%sreturn;\n", indent(indLevel));
                lines.push_back(retStmt);
                return;
            }

            // generate code for its successor if it hasn't already been 
            // visited and is in the same loop/case and is not the latch 
            // for the current most enclosing loop.  The only exception 
            // for generating it when it is not in the same loop is when 
            // it is only reached from this node
            PBB child = m_OutEdges[0];
            if (child->traversed == DFS_CODEGEN || 
                ((child->loopHead != loopHead) && 
		 (!child->allParentsGenerated() || 
                  isIn(followSet, child))) ||
	        (latch && latch->loopHead->loopFollow == child) ||
		!(caseHead == child->caseHead || 
                  (caseHead && child == caseHead->condFollow)))
                emitGotoAndLabel(lines, indLevel, m_OutEdges[0]);
            else
                m_OutEdges[0]->generateCode(lines, indLevel, latch,
                     followSet, gotoSet);
	    break;
    }
}

void BasicBlock::getLiveInAt(Statement *stmt, std::set<Statement*> &livein) {
    getLiveIn(livein);
    for (std::list<RTL*>::iterator rit = m_pRtls->begin(); 
      rit != m_pRtls->end(); rit++) {
        RTL *rtl = *rit;
        for (std::list<Exp*>::iterator it = rtl->getList().begin(); 
          it != rtl->getList().end(); it++) {
            if (*it == (AssignExp*)stmt) return;
            Statement *e = dynamic_cast<Statement*>(*it);
            if (e == NULL) continue;
            e->setBB(this);
            e->calcLiveOut(livein);
        }
        if (rtl->getKind() == CALL_RTL) {
            HLCall *call = (HLCall*)rtl;
            if (call == stmt) return;
            call->setBB(this);
            call->calcLiveOut(livein);
            std::list<Statement*> &stmts = call->getInternalStatements();
            for (std::list<Statement*>::iterator it1 = stmts.begin();
              it1 != stmts.end(); it1++) {
                if (stmt == *it1) return;
                (*it1)->setBB(this);
                (*it1)->calcLiveOut(livein);
            } 
        }
        if (rtl->getKind() == JCOND_RTL) {
            HLJcond *jcond = (HLJcond*)rtl;
            if (jcond == stmt) return;
            jcond->setBB(this);
            jcond->calcLiveOut(livein);
        }
    }
}

void BasicBlock::calcLiveOut(std::set<Statement*> &live) {
    /* hopefully we can be sure that NULL is not a valid assignment,
       so this will calculate the live set after every assignment */
    getLiveInAt(NULL, live);
}

void BasicBlock::getLiveIn(std::set<Statement*> &livein) {
    for (unsigned i = 0; i < m_InEdges.size(); i++) {
        std::set<Statement*> &in = m_InEdges[i]->getLiveOut();
        // set union, C++ doesn't have one!
        for (std::set<Statement*>::iterator it = in.begin(); 
          it != in.end(); it++) {
            assert(*it);
            livein.insert(*it);
        }
    }
}

void BasicBlock::setLoopStamps(int &time, std::vector<PBB> &order)
{
    // timestamp the current node with the current time and set its traversed 
    // flag
    traversed = DFS_LNUM;
    loopStamps[0] = time;

    // recurse on unvisited children and set inedges for all children
    for (unsigned int i = 0; i < m_OutEdges.size(); i++) {
        // set the in edge from this child to its parent (the current node)
        // (not done here, might be a problem)
        // outEdges[i]->inEdges.Add(this);

        // recurse on this child if it hasn't already been visited
        if (m_OutEdges[i]->traversed != DFS_LNUM)
            m_OutEdges[i]->setLoopStamps(++time, order);
    }

    // set the the second loopStamp value
    loopStamps[1] = ++time;

    // add this node to the ordering structure as well as recording its 
    // position within the ordering
    ord = order.size();
    order.push_back(this);
}

void BasicBlock::setRevLoopStamps(int &time)
{
    // timestamp the current node with the current time and set its traversed 
    // flag
    traversed = DFS_RNUM;
    revLoopStamps[0] = time;

    // recurse on the unvisited children in reverse order
    for (int i = m_OutEdges.size() - 1; i >= 0; i--) {
        // recurse on this child if it hasn't already been visited
        if (m_OutEdges[i]->traversed != DFS_RNUM)
            m_OutEdges[i]->setRevLoopStamps(++time);
    }

    // set the the second loopStamp value
    revLoopStamps[1] = ++time;
}

void BasicBlock::setRevOrder(std::vector<PBB> &order)
{
    // Set this node as having been traversed during the post domimator 
    // DFS ordering traversal
    traversed = DFS_PDOM;
	
    // recurse on unvisited children 
    for (unsigned int i = 0; i < m_InEdges.size(); i++)
	if (m_InEdges[i]->traversed != DFS_PDOM)
	    m_InEdges[i]->setRevOrder(order);

    // add this node to the ordering structure and record the post dom. order
    // of this node as its index within this ordering structure
    revOrd = order.size();
    order.push_back(this);
}

void BasicBlock::setCaseHead(PBB head, PBB follow) 
{
    assert(!caseHead);

    traversed = DFS_CASE;

    // don't tag this node if it is the case header under investigation 
    if (this != head)
        caseHead = head;

    // if this is a nested case header, then it's member nodes will already 
    // have been tagged so skip straight to its follow
    if (getType() == NWAY && this != head) {
        if (condFollow->traversed != DFS_CASE && condFollow != follow)
            condFollow->setCaseHead(head, follow);
    } else
        // traverse each child of this node that:
        //   i) isn't on a back-edge,
        //  ii) hasn't already been traversed in a case tagging traversal and,
        // iii) isn't the follow node.
        for (unsigned int i = 0; i < m_OutEdges.size(); i++)
            if (!hasBackEdgeTo(m_OutEdges[i]) && 
                m_OutEdges[i]->traversed != DFS_CASE && 
                m_OutEdges[i] != follow)
                m_OutEdges[i]->setCaseHead(head, follow);
}

void BasicBlock::setStructType(structType s)
{
    // if this is a conditional header, determine exactly which type of 
    // conditional header it is (i.e. switch, if-then, if-then-else etc.)
    if (s == Cond) {
        if (getType() == NWAY) 
            cType = Case;
        else if (m_OutEdges[BELSE] == condFollow)
            cType = IfThen;
        else if (m_OutEdges[BTHEN] == condFollow)
            cType = IfElse;
        else
            cType = IfThenElse;
    }

    sType = s;
}

void BasicBlock::setUnstructType(unstructType us)
{
    assert((sType == Cond || sType == LoopCond) && cType != Case);
    usType = us;
}

unstructType BasicBlock::getUnstructType()
{
    assert((sType == Cond || sType == LoopCond) && cType != Case);
    return usType;
}

void BasicBlock::setLoopType(loopType l)
{
    assert (sType == Loop || sType == LoopCond);
    lType = l;

    // set the structured class (back to) just Loop if the loop type is 
    // PreTested OR it's PostTested and is a single block loop
    if (lType == PreTested || (lType == PostTested && this == latchNode))
        sType = Loop;
}

loopType BasicBlock::getLoopType()
{
    assert (sType == Loop || sType == LoopCond);
    return lType;
}

void BasicBlock::setCondType(condType c)
{
    assert (sType == Cond || sType == LoopCond);
    cType = c;
}

condType BasicBlock::getCondType()
{
    assert (sType == Cond || sType == LoopCond);
    return cType;
}

bool BasicBlock::inLoop(PBB header, PBB latch)
{
   assert(header->latchNode == latch);
   assert(header == latch || 
          ((header->loopStamps[0] > latch->loopStamps[0] && 
            latch->loopStamps[1] > header->loopStamps[1]) ||
	  (header->loopStamps[0] < latch->loopStamps[0] && 
           latch->loopStamps[1] < header->loopStamps[1])));
   // this node is in the loop if it is the latch node OR
   // this node is within the header and the latch is within this when using 
   // the forward loop stamps OR
   // this node is within the header and the latch is within this when using 
   // the reverse loop stamps 
   return this == latch ||
          (header->loopStamps[0] < loopStamps[0] && 
           loopStamps[1] < header->loopStamps[1] &&
           loopStamps[0] < latch->loopStamps[0] && 
           latch->loopStamps[1] < loopStamps[1]) ||
          (header->revLoopStamps[0] < revLoopStamps[0] && 
           revLoopStamps[1] < header->revLoopStamps[1] &&
           revLoopStamps[0] < latch->revLoopStamps[0] && 
           latch->revLoopStamps[1] < revLoopStamps[1]);
}

