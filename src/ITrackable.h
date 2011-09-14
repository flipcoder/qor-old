#ifndef _TRACKABLE_H
#define _TRACKABLE_H

class ITrackable
{
	protected:

		std::list<ITrackable*> m_Trackers; // objects that are tracking us
		//std::list<ITrackable*> m_Track; // objects that we're tracking

	public:

		ITrackable() {}
		virtual ~ITrackable() {
			// notify trackers of deletion
			//foreach(ITrackable* t, m_Trackers)
			//    t->removeTracker(t);
		}

		void addTracker(ITrackable* t) {
			m_Trackers.push_back(t);
		}
		
		void clearTrackers() {
			m_Trackers.clear();	
		}
		
		void removeTracker(ITrackable* t) {
			for(std::list<ITrackable*>::iterator itr = m_Trackers.begin();
				itr != m_Trackers.end();
				++itr)
			{
				if(*itr == t) {
					m_Trackers.erase(itr);
					break;
				}
			}
			//_onRemoveTracker();
		}

		//virtual void _onRemoveTracker(ITrackable* t) = 0;
};

#endif

