class Service(object):

    def __init__(self, raw_service):
        self.uuid = raw_service.Uuid().decode()
        self.handle = raw_service.Handle()
        self.group_end_handle = raw_service.GroupEndHandle()

    def __str__(self):
        return "<Service uuid={}, start_handle={}, end_handle={}>"\
            .format(self.uuid, self.handle, self.group_end_handle)
