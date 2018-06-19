class Service(object):

    def __init__(self, raw_service):
        self.handle = raw_service.Handle()
        self.group_end_handle = raw_service.GroupEndHandle()
        self.uuid = raw_service.Uuid().decode()
